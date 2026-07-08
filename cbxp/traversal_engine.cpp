#include "traversal_engine.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>

#include "control_block_error.hpp"
#include "control_block_node.hpp"
#include "include_resolver.hpp"
#include "logger.hpp"

namespace CBXP {

namespace {

std::string addressToString(const void* addr) {
  std::ostringstream oss;
  oss << addr;
  return oss.str();
}

}  // namespace

TraversalEngine::TraversalEngine(
    const std::unordered_map<std::string, ControlBlock>& control_blocks,
    bool skip_buffer_length_check)
    : ctx_{control_blocks, skip_buffer_length_check, {}, {}, {}, {}} {}

// ---------------------------------------------------------------------------
// Public

nlohmann::json TraversalEngine::run(const std::string& cb_name,
                                    const cbxp_options_t& options,
                                    const void* address, size_t buffer_length) {
  const ControlBlock& cb = ctx_.control_blocks.at(cb_name);
  // Validate the include and filter options against the root control block
  // before touching any storage, so a bad option is reported immediately
  // rather than depending on when (or whether) the node is reached.  The
  // constructor resolves includes and parses filters but reads no memory; it
  // throws IncludeError / FilterError on invalid input.
  // The node is kept and handed to drainQueue for each root, so the include
  // and filter lists are built (and logged) only once.
  const ControlBlockNode validator(ctx_, options, cb_name, /*address=*/nullptr,
                                   buffer_length);

  // Reachability check applies when we are either extracting (address ==
  // nullptr) or descending recursively into a child (the engine calls itself
  // for child nodes via drainQueue, where address is always non-null — we
  // skip the storage-attribute check in that case because the parent already
  // passed it).
  if (address == nullptr) {
    checkReachability(cb, !options.filters.empty());
  }

  // Resolve the set of root addresses for this traversal.
  std::vector<const void*> roots;
  if (address != nullptr) {
    roots = {address};
  } else {
    roots = findRootAddresses(cb);
    // Deduplicate (e.g. ECVT is pointed to by both CVT and PSA).
    std::vector<const void*> unique_roots;
    for (const void* p : roots) {
      if (std::find(unique_roots.begin(), unique_roots.end(), p) ==
          unique_roots.end()) {
        unique_roots.push_back(p);
      }
    }
    roots = std::move(unique_roots);
  }

  if (roots.empty()) {
    Logger::getInstance().debug("No pointers found for '" + cb_name +
                                "' control block.");
    return {};
  }

  // Seed the BFS queue with the root node(s).  Mark is_root=true so that
  // drainQueue applies the full reachability check (including the
  // no-filter-for-non-common-storage guard) only for these entries.
  for (const void* root_addr : roots) {
    auto visit_key = std::make_pair(root_addr, cb_name);
    // try_emplace is a no-op when the key already exists, so this
    // atomically checks-and-inserts the in-progress sentinel.
    if (ctx_.visited.try_emplace(visit_key, false).second) {
      ctx_.queue.push({root_addr, cb_name, options, /*is_root=*/true,
                       std::make_shared<ControlBlockNode>(
                           validator.withAddress(root_addr))});
    }
  }

  // Process the queue until empty — this is the BFS loop.
  drainQueue();

  // Collect results for the root(s) and build the output JSON.
  nlohmann::json output;

  // Resolve placeholder hex strings to real child JSON.  A root is omitted
  // when it (or a filtered descendant) was rejected.
  auto resolveRoot = [&](const void* root_addr, nlohmann::json& resolved) {
    const VisitKey visit_key(root_addr, cb_name);
    const nlohmann::json& v = ctx_.visited.at(visit_key);
    if (v.is_null() || v.is_boolean()) {
      return false;
    }
    resolved = v;
    VisitKeySet expanded;
    const VisitKeyMap owners = computeOwners(visit_key);
    return resolvePlaceholders(resolved, visit_key, expanded, owners);
  };

  if (roots.size() == 1) {
    nlohmann::json resolved;
    if (resolveRoot(roots[0], resolved)) {
      output[cb_name] = resolved;
    }
  } else {
    output[cb_name + "s"] = nlohmann::json::array();
    for (const void* root_addr : roots) {
      nlohmann::json resolved;
      if (resolveRoot(root_addr, resolved)) {
        output[cb_name + "s"].push_back(resolved);
      }
    }
  }

  return output;
}

// ---------------------------------------------------------------------------
// Private

bool TraversalEngine::isPsaProtectedAddress(const void* addr) {
  // The z/OS PSA prefix area 0x7FFFF000–0x7FFFFFFF is fetch-protected and
  // will cause a 0C4 if read from a non-zero key.  Reject any pointer that
  // falls in this range before attempting any memory access.
  const uintptr_t p  = reinterpret_cast<uintptr_t>(addr);
  const uintptr_t lo = 0x7FFFF000u;
  const uintptr_t hi = 0x7FFFFFFFu;
  return (p >= lo && p <= hi);
}

void TraversalEngine::checkReachability(const ControlBlock& cb,
                                        bool has_filters) {
  const storage_attributes_t& attr = cb.getStorageAttributes();
  if (attr.key != 8 && attr.is_protected && cb.getName() != "psa") {
    Logger::getInstance().debug(
        "The '" + cb.getName() + "' control block is key '" +
        std::to_string(attr.key) +
        "' and fetch protected, so cbxp cannot access it.");
    throw CbxpReachBlockError();
  }
  if (!attr.is_common && !has_filters) {
    Logger::getInstance().debug(
        "The '" + cb.getName() +
        "' control block is not in common storage and there are no filters "
        "enabled, so cbxp cannot guarantee accuracy.");
    throw CbxpReachBlockError();
  }
}

std::vector<const void*> TraversalEngine::findRootAddresses(
    const ControlBlock& cb) const {
  std::vector<const void*> result;
  const std::vector<std::string>& pointed_to_by = cb.getPointedToBy();

  if (pointed_to_by.empty()) {
    return {cb.getFixedAddress()};
  }

  for (const auto& parent_name : pointed_to_by) {
    const ControlBlock& parent_cb = ctx_.control_blocks.at(parent_name);
    const std::vector<const void*> parent_addrs = findRootAddresses(parent_cb);

    // Find the field in parent that points to cb.
    std::string pointer_field_name;
    ptrdiff_t pointer_offset = 0;
    size_t pointer_length    = 4;
    std::string count_field;

    for (const auto& [fname, fdata] : parent_cb.getMap()) {
      if (!fdata.pointsTo.empty() && fdata.pointsTo == cb.getName()) {
        pointer_field_name = fname;
        pointer_offset     = fdata.offset;
        pointer_length     = fdata.length;
        count_field        = fdata.count;
        break;
      }
    }

    for (const void* parent_addr : parent_addrs) {
      Logger::getInstance().debug("Polling '" + parent_name + "' field '" +
                                  pointer_field_name + "' at offset '" +
                                  std::to_string(pointer_offset) +
                                  "' for control block tree...");

      size_t count = 1;
      if (!count_field.empty()) {
        std::string count_key = count_field;
        std::transform(count_key.begin(), count_key.end(), count_key.begin(),
                       [](unsigned char c) { return std::toupper(c); });
        const control_block_field_t& count_f = parent_cb.getMap().at(count_key);
        const char* count_ptr =
            static_cast<const char*>(parent_addr) + count_f.offset;
        if (count_f.type == SIGNED_INT) {
          int64_t signed_count =
              (count_f.length == 8)
                  ? *(reinterpret_cast<const int64_t*>(count_ptr))
                  : *(reinterpret_cast<const int32_t*>(count_ptr));
          count = (signed_count > 0) ? static_cast<size_t>(signed_count) : 0;
        } else {
          count = (count_f.length == 8)
                      ? *(reinterpret_cast<const uint64_t*>(count_ptr))
                      : *(reinterpret_cast<const uint32_t*>(count_ptr));
        }
        Logger::getInstance().debug(
            "Repeated pointer field '" + pointer_field_name +
            "' count_field='" + count_key + "' count=" + std::to_string(count));
      }

      for (size_t i = 0; i < count; ++i) {
        const char* base = static_cast<const char*>(parent_addr) +
                           i * pointer_length + pointer_offset;
        const void* resolved;
        if (pointer_length == 8) {
          resolved = reinterpret_cast<const void*>(
              *reinterpret_cast<const uint64_t*>(base));
        } else {
          resolved = reinterpret_cast<const void* __ptr32>(
              *reinterpret_cast<const uint32_t*>(base));
        }

        // Skip entries masked by ignoreMask.
        const auto& field_map = parent_cb.getMap();
        auto mask_it          = field_map.find(pointer_field_name);
        if (mask_it != field_map.end() && !mask_it->second.ignoreMask.empty()) {
          uint32_t raw_val = *reinterpret_cast<const uint32_t*>(base);
          uint32_t mask    = static_cast<uint32_t>(std::stoull(
                              mask_it->second.ignoreMask, nullptr, 16))
                          << 24;
          if (raw_val & mask) {
            Logger::getInstance().debug(
                "Skipping '" + cb.getName() + "' entry at index " +
                std::to_string(i) + " — ignoreMask bit set (raw=0x" +
                [&]() {
                  std::ostringstream oss;
                  oss << std::hex << raw_val;
                  return oss.str();
                }() +
                ")");
            continue;
          }
        }

        Logger::getInstance().debug("Resolved '" + cb.getName() +
                                    "' pointer to address " + [&]() {
                                      std::ostringstream oss;
                                      oss << resolved;
                                      return oss.str();
                                    }());
        result.push_back(resolved);
      }
    }
  }

  return result;
}

void TraversalEngine::drainQueue() {
  while (!ctx_.queue.empty()) {
    PendingNode node = ctx_.queue.front();
    ctx_.queue.pop();

    const ControlBlock& cb = ctx_.control_blocks.at(node.cb_name);

    // For root nodes apply the full reachability check, which includes
    // refusing access to non-common storage when no filters are present.
    // For child nodes (enqueued by fieldToJson via a valid parent pointer)
    // we still check the key/fetch-protection gate but treat the absence of
    // filters as acceptable — the child was explicitly included by the user
    // and its address was read from a live parent field.
    auto visit_key = std::make_pair(node.address, node.cb_name);

    // Only CbxpReachBlockError is handled per node: an inaccessible block
    // (protected storage, bad pointer from a peer-ASCB link like ASCBLOCI,
    // etc.) is recorded as null so the placeholder stays as raw hex.  Every
    // other CBXPError (bad include/filter, data too small, ...) propagates
    // out of run() to ControlBlockExplorer::processControlBlock, which
    // reports it to the caller as the return code.
    try {
      if (isPsaProtectedAddress(node.address)) {
        Logger::getInstance().debug("Skipping '" + node.cb_name +
                                    "' — address falls in PSA fetch-protected "
                                    "range 0x7FFFF000–0x7FFFFFFF");
        throw CbxpReachBlockError();
      }
      if (node.is_root) {
        checkReachability(cb, !node.options.filters.empty());
      } else {
        checkReachability(cb, /*has_filters=*/true);
      }

      Logger::getInstance().debug(node.cb_name + " hex dump: address=" + [&]() {
        std::ostringstream oss;
        oss << node.address;
        return oss.str();
      }() + " length=" + std::to_string(cb.getMaxOffset()));
      Logger::getInstance().hexDump(
          reinterpret_cast<const char*>(node.address),
          static_cast<int>(cb.getName() == "psa" ? (cb.getMaxOffset() - 8) / 2
                                                 : cb.getMaxOffset()),
          cb.getName() == "psa");

      // Root nodes arrive with the node run() built while validating the
      // options; children are built here.
      std::shared_ptr<ControlBlockNode> cb_node = node.prepared;
      if (!cb_node) {
        cb_node = std::make_shared<ControlBlockNode>(
            ctx_, node.options, node.cb_name, node.address,
            0 /* buffer_length: 0 = live memory */);
      }
      NodeParseResult result = cb_node->parse();

      if (result.json.is_null()) {
        ctx_.visited[visit_key] = nullptr;
      } else if (!result.filter_passed) {
        // Remember that this node was rejected by a filter so that its
        // parent can tell this apart from an inaccessible node.
        Logger::getInstance().debug(
            "Rejected '" + node.cb_name + "' at address " +
            addressToString(node.address) + ": did not pass its filters");
        ctx_.visited[visit_key] = nullptr;
        ctx_.filter_rejected.insert(visit_key);
      } else {
        ctx_.visited[visit_key]        = result.json;
        ctx_.child_filtered[visit_key] = cb_node->childFilterMap();
      }
    } catch (const CbxpReachBlockError& e) {
      Logger::getInstance().debug(
          "Skipping '" + node.cb_name + "' at address " +
          addressToString(node.address) + " — error code " +
          std::to_string(e.getErrorCode()));
      ctx_.visited[visit_key] = nullptr;
    }
  }
}

namespace {

enum class ChildState { Resolved, Rejected, Unavailable };

const void* placeholderToAddress(const std::string& hex_str,
                                 size_t field_length) {
  if (field_length == 8) {
    return reinterpret_cast<const void*>(std::stoull(hex_str, nullptr, 0));
  }
  return reinterpret_cast<const void* __ptr32>(
      static_cast<uint32_t>(std::stoull(hex_str, nullptr, 0)));
}

struct ChildField {
  std::string name;
  size_t length    = 4;
  bool is_repeated = false;
};

// Determines whether JSON `key` of control block `cb` is a pointer field
// whose target is an included child.  Lower-case field names are matched
// against the key.  Repeated pointer fields are stored under "<field>s" (with
// a trailing "s"), so both the exact (scalar) and the repeated (array) forms
// are checked.
bool findChildField(const ControlBlock& cb, const std::string& key,
                    const std::unordered_map<std::string, bool>& child_options,
                    ChildField& out) {
  for (const auto& [fname, fdata] : cb.getMap()) {
    std::string lower_fname = fname;
    std::transform(lower_fname.begin(), lower_fname.end(), lower_fname.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    const bool exact_match    = (lower_fname == key);
    const bool repeated_match = (lower_fname + "s" == key) && fdata.repeated;
    if ((exact_match || repeated_match) && !fdata.pointsTo.empty() &&
        child_options.find(fdata.pointsTo) != child_options.end()) {
      out = {fdata.pointsTo, fdata.length, repeated_match};
      return true;
    }
  }
  return false;
}

bool isPtrKey(const std::string& key) {
  return key.size() > 4 && key.substr(key.size() - 4) == "_ptr";
}

}  // namespace

VisitKeyMap TraversalEngine::computeOwners(const VisitKey& root_key) const {
  VisitKeyMap owners;
  owners.emplace(root_key, root_key);
  std::queue<VisitKey> pending;
  pending.push(root_key);

  while (!pending.empty()) {
    const VisitKey node_key = pending.front();
    pending.pop();

    auto node_it = ctx_.visited.find(node_key);
    auto opts_it = ctx_.child_filtered.find(node_key);
    if (node_it == ctx_.visited.end() || opts_it == ctx_.child_filtered.end() ||
        !node_it->second.is_object()) {
      continue;
    }
    const ControlBlock& cb = ctx_.control_blocks.at(node_key.second);

    auto claim = [&](const nlohmann::json& placeholder, const ChildField& f) {
      if (!placeholder.is_string()) {
        return;
      }
      const VisitKey child_key(
          placeholderToAddress(placeholder.get<std::string>(), f.length),
          f.name);
      if (owners.emplace(child_key, node_key).second) {
        pending.push(child_key);
      }
    };

    for (const auto& [key, value] : node_it->second.items()) {
      ChildField field;
      if (isPtrKey(key) || !findChildField(cb, key, opts_it->second, field)) {
        continue;
      }
      if (field.is_repeated && value.is_array()) {
        for (const auto& elem : value) {
          claim(elem, field);
        }
      } else if (!field.is_repeated) {
        claim(value, field);
      }
    }
  }
  return owners;
}

// Stitches child JSON into `json` (the parsed node identified by `node_key`)
// and decides whether the node survives its children's filters.
//
// Two cases per included child:
//  - Unfiltered child: looked up in ctx_.visited and its own included
//    children are stitched in recursively, so multi-level includes such as
//    'assb.ldax' are fully expanded.  It can never reject this node.  A node
//    is expanded only under its owner, the shallowest parent that reaches it
//    (`owners`, from computeOwners), and at most once per root (`expanded`);
//    every other reference stays as its pointer placeholder.  Because the
//    owner is the BFS parent, entries of a repeated field such as 'asvtenty'
//    are expanded in place instead of being swallowed by a deeper path (e.g.
//    ascb -> oucb -> oucbbck -> ...).  This also keeps the work linear on
//    cyclic and heavily shared graphs (e.g. with '**'), where expanding every
//    path would never finish.
//  - Filtered child: resolved recursively (bottom-up) so that its own
//    filtered descendants can reject it.  Filters lose one path segment per
//    level, so recursion along filtered edges is bounded by filter depth and
//    cannot cycle.
//
// Returns false if the node must be rejected because a filtered child was
// rejected (or could not be evaluated).  For repeated pointer fields,
// rejected entries are dropped and the node is rejected only when every entry
// was rejected.
bool TraversalEngine::resolvePlaceholders(nlohmann::json& json,
                                          const VisitKey& node_key,
                                          VisitKeySet& expanded,
                                          const VisitKeyMap& owners) const {
  if (!json.is_object()) {
    return true;
  }

  expanded.insert(node_key);

  const std::string& cb_name = node_key.second;
  const ControlBlock& cb     = ctx_.control_blocks.at(cb_name);
  // Included child name -> whether that child carries filters.
  const auto& child_options = ctx_.child_filtered.at(node_key);
  const std::string node_desc =
      "'" + cb_name + "' at address " + addressToString(node_key.first);

  for (auto& [key, value] : json.items()) {
    // Skip the "_ptr" preservation keys.
    if (isPtrKey(key)) {
      continue;
    }

    // Determine whether this key maps to a pointer field with an included
    // child.
    ChildField child_field;
    if (!findChildField(cb, key, child_options, child_field)) {
      continue;
    }
    const std::string& child_name = child_field.name;
    const size_t field_length     = child_field.length;
    const bool is_repeated_key    = child_field.is_repeated;

    // Only children that carry filters can reject this node.
    const bool child_filtered = child_options.at(child_name);

    // Look up one child placeholder.  On Resolved, child_json holds the child
    // (for filtered children, with its own filtered descendants stitched in).
    auto resolveChild = [&](const std::string& hex_str,
                            nlohmann::json& child_json) {
      const void* child_addr = placeholderToAddress(hex_str, field_length);
      const VisitKey child_key(child_addr, child_name);

      if (!child_filtered) {
        // Expand only under the owner (shallowest parent); any other
        // reference stays as a pointer placeholder.
        auto owner_it = owners.find(child_key);
        auto it       = ctx_.visited.find(child_key);
        if (owner_it == owners.end() || owner_it->second != node_key ||
            it == ctx_.visited.end() || it->second.is_boolean() ||
            it->second.is_null() || expanded.count(child_key) != 0) {
          return ChildState::Unavailable;
        }
        child_json = it->second;
        resolvePlaceholders(child_json, child_key, expanded, owners);
        return ChildState::Resolved;
      }

      ChildState state = ChildState::Unavailable;
      if (ctx_.filter_rejected.count(child_key) != 0) {
        state = ChildState::Rejected;
      } else {
        auto it = ctx_.visited.find(child_key);
        if (it != ctx_.visited.end() && !it->second.is_boolean() &&
            !it->second.is_null()) {
          child_json = it->second;
          state = resolvePlaceholders(child_json, child_key, expanded, owners)
                      ? ChildState::Resolved
                      : ChildState::Rejected;
        }
      }

      const char* verdict = state == ChildState::Resolved ? "passed"
                            : state == ChildState::Rejected
                                ? "was REJECTED by a filter"
                                : "is unavailable (treated as rejected)";
      Logger::getInstance().debug(
          "Parent " + node_desc + " evaluated filtered child '" + child_name +
          "' at address " + addressToString(child_addr) + ": " + verdict);
      return state;
    };

    if (!is_repeated_key) {
      if (!value.is_string()) {
        continue;
      }
      nlohmann::json child_json;
      ChildState state = resolveChild(value.get<std::string>(), child_json);
      if (state == ChildState::Resolved) {
        json[key + "_ptr"] = value;
        value              = child_json;
      } else if (child_filtered) {
        Logger::getInstance().debug("Rejecting " + node_desc + ": child via '" +
                                    key + "' did not pass its filters");
        return false;
      }
    } else if (value.is_array()) {
      nlohmann::json kept = nlohmann::json::array();
      size_t rejected     = 0;
      for (auto& elem : value) {
        if (!elem.is_string()) {
          kept.push_back(elem);
          continue;
        }
        nlohmann::json child_json;
        ChildState state = resolveChild(elem.get<std::string>(), child_json);
        if (state == ChildState::Resolved) {
          kept.push_back(child_json);
        } else if (child_filtered) {
          ++rejected;
        } else {
          kept.push_back(elem);
        }
      }
      if (rejected > 0) {
        Logger::getInstance().debug(
            "Parent " + node_desc + " dropped " + std::to_string(rejected) +
            " rejected entr" + (rejected == 1 ? "y" : "ies") + " from '" + key +
            "' (" + std::to_string(kept.size()) + " kept)");
      }
      if (rejected > 0 && kept.empty()) {
        Logger::getInstance().debug("Rejecting " + node_desc +
                                    ": every entry in '" + key +
                                    "' was rejected by child filters");
        return false;
      }
      value = std::move(kept);
    }
  }

  return true;
}

}  // namespace CBXP
