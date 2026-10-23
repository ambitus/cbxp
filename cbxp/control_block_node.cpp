#include "control_block_node.hpp"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <sstream>

#include "control_block_error.hpp"
#include "logger.hpp"

namespace CBXP {

ControlBlockNode::ControlBlockNode(TraversalContext& ctx,
                                   const cbxp_options_t& options,
                                   const std::string& cb_name,
                                   const void* address, size_t buffer_length)
    : ctx_(ctx),
      cb_(ctx.control_blocks.at(cb_name)),
      address_(address),
      buffer_length_(buffer_length) {
  // Resolve include patterns into child_options_ for this node.
  child_options_ = IncludeResolver::resolve(options.include_patterns, cb_,
                                            ctx_.control_blocks);
  // Propagate skip_buffer_length_check into each child option set.
  for (auto& [name, child_opt] : child_options_) {
    child_opt.skip_buffer_length_check = ctx_.skip_buffer_length_check;
  }
  // Parse filter strings; child filters are pushed into child_options_.
  filter_set_.parse(options.filters, child_options_, cb_name);
}

// ---------------------------------------------------------------------------
// Public

NodeParseResult ControlBlockNode::parse() {
  NodeParseResult result;
  result.filter_passed       = true;
  result.child_filter_failed = false;
  result.json                = nullptr;

  // Verify the entire control block fits in the caller-supplied buffer before
  // reading anything (including the eyecatcher).  checkDataLength is a no-op
  // when skip_buffer_length_check is true (extract / live-memory path).
  checkDataLength(getReadableLength());

  if (!eyecatcherMatches()) {
    return result;  // json stays null
  }

  result.json = parseFields(result.child_filter_failed);

  if (!filter_set_.match(result.json, result.child_filter_failed)) {
    result.filter_passed = false;
    pending_children_.clear();  // rejected: do not load children
    return result;
  }

  enqueuePendingChildren();

  return result;
}

ControlBlockNode ControlBlockNode::withAddress(const void* address) const {
  ControlBlockNode copy(*this);
  copy.address_ = address;
  return copy;
}

std::unordered_map<std::string, bool> ControlBlockNode::childFilterMap() const {
  std::unordered_map<std::string, bool> out;
  for (const auto& [name, child_opt] : child_options_) {
    out[name] = !child_opt.filters.empty();
  }
  return out;
}

// ---------------------------------------------------------------------------
// Private helpers

bool ControlBlockNode::eyecatcherMatches() const {
  const eyecatcher_field_t& ec = cb_.getEyecatcherField();
  if (ec.expected.empty()) {
    return true;
  }
  const char* raw = static_cast<const char*>(address_) + ec.offset;
  std::string actual =
      ControlBlockFieldFormatter::getString(raw, static_cast<int>(ec.length));
  std::string expected = ec.expected;
  expected.erase(expected.find_last_not_of(" \t\0", std::string::npos, 3) + 1);
  if (actual != expected) {
    Logger::getInstance().debug(
        "Eyecatcher mismatch for '" + cb_.getName() + "' at " +
        [&]() {
          std::ostringstream oss;
          oss << address_;
          return oss.str();
        }() +
        ": expected '" + expected + "', got '" + actual + "' — skipping");
    return false;
  }
  return true;
}

void ControlBlockNode::checkDataLength(ptrdiff_t offset) const {
  if (ctx_.skip_buffer_length_check) {
    return;
  }
  if (offset < 0) {
    return;
  }
  Logger::getInstance().debug(
      "Checking if specified buffer (" + std::to_string(buffer_length_) +
      " bytes) too small to contain the '" + cb_.getName() +
      "' control block (requires at least " + std::to_string(offset) +
      " bytes)...");
  if (static_cast<size_t>(offset) > buffer_length_) {
    throw DataLengthError();
  }
}

ptrdiff_t ControlBlockNode::getReadableLength() const {
  return cb_.getName() == "psa" ? (cb_.getMaxOffset() - 8) / 2
                                : cb_.getMaxOffset();
}

nlohmann::json ControlBlockNode::fieldToJson(const control_block_field_t& field,
                                             ptrdiff_t extra_offset) {
  ptrdiff_t offset = field.offset + extra_offset;
  checkDataLength(offset);

  nlohmann::json field_json;
  const char* base = static_cast<const char*>(address_);

  try {
    switch (field.type) {
      case STRING:
        field_json = cb_.formatter_.getString(base + offset, field.length);
        break;
      case UNSIGNED_INT:
        if (field.length == 8) {
          field_json = cb_.formatter_.uint<uint64_t>(base + offset);
        } else {
          field_json = cb_.formatter_.uint<uint32_t>(base + offset);
        }
        break;
      case SIGNED_INT:
        if (field.length == 8) {
          field_json = *(reinterpret_cast<const int64_t*>(base + offset));
        } else if (field.length == 2) {
          field_json = *(reinterpret_cast<const int16_t*>(base + offset));
        } else {
          field_json = *(reinterpret_cast<const int32_t*>(base + offset));
        }
        break;
      case BITSTRING:
      case HEX:
      case ADDRESS:
        if (field.length == 8) {
          field_json = cb_.formatter_.getHex<uint64_t>(base + offset);
        } else {
          field_json = cb_.formatter_.getHex<uint32_t>(base + offset);
        }
        break;
    }
  } catch (const std::exception&) {
    Logger::getInstance().debug("Unexpected error reading field.");
    throw CbxpReachBlockError();
  }

  // If this field points to a child that we should include, push the target
  // onto the BFS queue and return the raw pointer hex as a placeholder.
  // The engine will replace placeholders after the queue drains.
  if (!field.pointsTo.empty() &&
      child_options_.find(field.pointsTo) != child_options_.end()) {
    const std::string& child_name = field.pointsTo;
    const char* raw               = base + offset;
    const void* child_addr;
    if (field.length == 8) {
      child_addr =
          reinterpret_cast<const void*>(cb_.formatter_.uint<uint64_t>(raw));
    } else {
      child_addr = reinterpret_cast<const void* __ptr32>(
          cb_.formatter_.uint<uint32_t>(raw));
    }

    // Reject addresses in the PSA fetch-protected range before enqueueing.
    // A hardware 0C4 fires before any C++ catch can intervene, so this must
    // be a pre-enqueue address check, not a try/catch.
    const uintptr_t p = reinterpret_cast<uintptr_t>(child_addr);
    if (p >= 0x7FFFF000u && p <= 0x7FFFFFFFu) {
      Logger::getInstance().debug("Skipping enqueue of '" + child_name +
                                  "' — address falls in PSA fetch-protected "
                                  "range 0x7FFFF000–0x7FFFFFFF");
      return field_json;  // leave raw hex placeholder in place
    }

    // Defer enqueueing until this node's own filters have been evaluated
    // (see enqueuePendingChildren), so children of rejected nodes are never
    // loaded.
    pending_children_.push_back({child_addr, child_name,
                                 child_options_[child_name],
                                 /*is_root=*/false});
    // Return the raw hex as a placeholder regardless — the engine will
    // patch it to the real child JSON once that node finishes processing.
  }

  return field_json;
}

void ControlBlockNode::enqueuePendingChildren() {
  for (const PendingNode& child : pending_children_) {
    auto visit_key       = std::make_pair(child.address, child.cb_name);
    std::string addr_str = [&]() {
      std::ostringstream oss;
      oss << child.address;
      return oss.str();
    }();
    if (ctx_.visited.find(visit_key) == ctx_.visited.end()) {
      // Mark as in-progress before enqueueing to prevent double-enqueue
      // from two parent nodes at the same BFS level both reaching the
      // same child address.
      ctx_.visited[visit_key] = false;  // sentinel
      ctx_.queue.push(child);
      Logger::getInstance().debug("Enqueued '" + child.cb_name +
                                  "' at address " + addr_str);
    } else {
      Logger::getInstance().debug("Already visited/enqueued '" + child.cb_name +
                                  "' at address " + addr_str);
    }
  }
  pending_children_.clear();
}

nlohmann::json ControlBlockNode::parseFields(bool& child_filter_failed) {
  child_filter_failed    = false;
  nlohmann::json cb_json = {};
  std::unordered_map<std::string, control_block_field_t> repeated_fields;

  for (const auto& [field_name, field_data] : cb_.getMap()) {
    if (field_data.endOfControlBlock) {
      continue;
    }
    if (cb_.getName() == "psa" &&
        field_data.offset >= (cb_.getMaxOffset() - 8) / 2 &&
        field_data.offset >= 0) {
      continue;
    }

    std::string lower_name = field_name;
    std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    Logger::getInstance().debug("Loading field '" + lower_name +
                                "' at offset '" +
                                std::to_string(field_data.offset) + "'");

    if (field_data.repeated) {
      repeated_fields[lower_name] = field_data;
      continue;
    }

    nlohmann::json resolved = fieldToJson(field_data);

    if (resolved.is_object() || resolved.is_array()) {
      // Already an expanded child — preserve raw pointer under "<name>_ptr".
      const char* raw = static_cast<const char*>(address_) + field_data.offset;
      cb_json[lower_name + "_ptr"] =
          (field_data.length == 8)
              ? nlohmann::json(cb_.formatter_.getHex<uint64_t>(raw))
              : nlohmann::json(cb_.formatter_.getHex<uint32_t>(raw));
    }
    cb_json[lower_name] = resolved;
  }

  // --- repeated fields ---
  for (const auto& [field_name, field_data] : repeated_fields) {
    nlohmann::json field_json    = {};
    field_json[field_name + "s"] = {};

    std::string count_key        = field_data.count;
    std::transform(count_key.begin(), count_key.end(), count_key.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    size_t count = cb_json[count_key].get<size_t>();
    Logger::getInstance().debug("Repeated field '" + field_name +
                                "': count_key='" + count_key +
                                "', count=" + std::to_string(count));

    const bool prior_child_filter_failed = child_filter_failed;
    bool any_entry_kept                  = false;
    bool any_entry_rejected              = false;

    for (size_t i = 0; i < count; ++i) {
      // Each repeated entry is treated as its own mini-node for filter
      // purposes.  We track whether this specific entry's child pointer
      // was rejected so we can decide whether to keep the entry.
      // We use a local flag and reset it between iterations.
      // Note: fieldToJson only enqueues; actual child-filter rejection is
      // determined by the engine after it processes the child node.  For
      // repeated pointer fields we therefore take the conservative approach:
      // if the child address was already visited and resolved to null (filter
      // rejected), treat this entry as rejected; otherwise keep it.
      // When the pointed-to control block is included, entries whose raw
      // value matches ignoreMask (e.g. unassigned ASVT slots) are dropped
      // before reading/enqueueing so they are neither emitted nor followed.
      if (!field_data.ignoreMask.empty() && !field_data.pointsTo.empty() &&
          child_options_.find(field_data.pointsTo) != child_options_.end()) {
        checkDataLength(field_data.offset +
                        static_cast<ptrdiff_t>(i) * field_data.length);
        const char* entry_raw = static_cast<const char*>(address_) +
                                field_data.offset + i * field_data.length;
        const uint32_t mask = static_cast<uint32_t>(std::stoull(
                                  field_data.ignoreMask, nullptr, 16))
                              << 24;
        if (cb_.formatter_.uint<uint32_t>(entry_raw) & mask) {
          continue;
        }
      }

      nlohmann::json entry = fieldToJson(
          field_data, static_cast<ptrdiff_t>(i) * field_data.length);

      if (!entry.is_null()) {
        field_json[field_name + "s"].push_back(entry);
        any_entry_kept = true;
      }
    }

    child_filter_failed =
        prior_child_filter_failed || (any_entry_rejected && !any_entry_kept);
    cb_json.merge_patch(field_json);
  }

  return cb_json;
}

}  // namespace CBXP
