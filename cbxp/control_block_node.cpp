#include "control_block_node.hpp"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <sstream>

#include "address_util.hpp"
#include "control_block_error.hpp"
#include "logger.hpp"

namespace CBXP {

ControlBlockNode::ControlBlockNode(TraversalContext& ctx,
                                   const cbxp_options_t& options,
                                   const std::string& cb_name,
                                   const void* p_address, size_t buffer_length)
    : ctx_(ctx),
      cb_(ctx.control_blocks.at(cb_name)),
      p_address_(p_address),
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
  result.filter_passed = true;
  result.json          = nullptr;

  // Verify the whole block fits in the caller's buffer before reading anything;
  // no-op when skip_buffer_length_check is true (live memory).
  ControlBlockNode::checkDataLength(ControlBlockNode::getReadableLength());

  if (!ControlBlockNode::eyecatcherMatches()) {
    return result;  // json stays null
  }

  result.json = ControlBlockNode::parseFields();

  if (!filter_set_.match(result.json)) {
    result.filter_passed = false;
    Logger::getInstance().debug(
        "Filters rejected '" + cb_.getName() + "' at address " +
        addressToString(p_address_) + ": discarding " +
        std::to_string(pending_children_.size()) + " pending child(ren)");
    pending_children_.clear();  // rejected: do not load children
    return result;
  }

  ControlBlockNode::enqueuePendingChildren();

  return result;
}

ControlBlockNode ControlBlockNode::withAddress(const void* p_address) const {
  ControlBlockNode copy(*this);
  copy.p_address_ = p_address;
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
  const char* p_raw = static_cast<const char*>(p_address_) + ec.offset;
  std::string actual =
      ControlBlockFieldFormatter::getString(p_raw, static_cast<int>(ec.length));
  std::string expected = ec.expected;
  expected.erase(expected.find_last_not_of(" \t\0", std::string::npos, 3) + 1);
  if (actual != expected) {
    Logger::getInstance().debug(
        "Eyecatcher mismatch for '" + cb_.getName() + "' at " +
        [&]() {
          std::ostringstream oss;
          oss << p_address_;
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
  ControlBlockNode::checkDataLength(offset);

  nlohmann::json field_json;
  const char* p_base = static_cast<const char*>(p_address_);

  try {
    switch (field.type) {
      case STRING:
        field_json = cb_.formatter_.getString(p_base + offset, field.length);
        break;
      case UNSIGNED_INT:
        if (field.length == 8) {
          field_json = cb_.formatter_.uint<uint64_t>(p_base + offset);
        } else if (field.length == 2) {
          field_json = cb_.formatter_.uint<uint16_t>(p_base + offset);
        } else if (field.length == 1) {
          field_json = cb_.formatter_.uint<uint8_t>(p_base + offset);
        } else {
          field_json = cb_.formatter_.uint<uint32_t>(p_base + offset);
        }
        break;
      case SIGNED_INT:
        if (field.length == 8) {
          field_json = cb_.formatter_.sint<int64_t>(p_base + offset);
        } else if (field.length == 2) {
          field_json = cb_.formatter_.sint<int16_t>(p_base + offset);
        } else if (field.length == 1) {
          field_json = cb_.formatter_.sint<int8_t>(p_base + offset);
        } else {
          field_json = cb_.formatter_.sint<int32_t>(p_base + offset);
        }
        break;
      case BITSTRING:
        if (field.length == 8) {
          field_json = cb_.formatter_.getBitmap<uint64_t>(p_base + offset);
        } else if (field.length == 2) {
          field_json = cb_.formatter_.getBitmap<uint16_t>(p_base + offset);
        } else if (field.length == 1) {
          field_json = cb_.formatter_.getBitmap<uint8_t>(p_base + offset);
        } else {
          field_json = cb_.formatter_.getBitmap<uint32_t>(p_base + offset);
        }
        break;
      case HEX:
      case ADDRESS:
        if (field.length == 8) {
          field_json = cb_.formatter_.getHex<uint64_t>(p_base + offset);
        } else {
          field_json = cb_.formatter_.getHex<uint32_t>(p_base + offset);
        }
        break;
    }
  } catch (const std::exception& e) {
    Logger::getInstance().debug(
        "Unexpected error reading field '" + field.name + "' (type " +
        std::to_string(static_cast<int>(field.type)) + ", offset " +
        std::to_string(offset) + ") of '" + cb_.getName() + "' at address " +
        addressToString(p_address_) + ": " + e.what());
    throw CbxpBlockUnreachableError();
  }

  // For an included child pointer field: enqueue the target and return the raw
  // pointer hex as a placeholder, which the engine resolves after traversal.
  if (!field.points_to.empty() &&
      child_options_.find(field.points_to) != child_options_.end()) {
    const std::string& child_name = field.points_to;
    const char* p_raw             = p_base + offset;
    const void* p_child_addr;
    if (field.length == 8) {
      p_child_addr =
          reinterpret_cast<const void*>(cb_.formatter_.uint<uint64_t>(p_raw));
    } else {
      p_child_addr = reinterpret_cast<const void* __ptr32>(
          cb_.formatter_.uint<uint32_t>(p_raw));
    }

    // Reject guard page addresses before enqueueing: the 0C4 fires
    // before any C++ catch, so a try/catch can't handle it.
    if (isGuardPageAddress(p_child_addr)) {
      Logger::getInstance().debug(
          "Skipping enqueue of '" + child_name +
          "' — address falls in PSA fetch-protected range " + kGuardPageRange);
      return field_json;  // leave raw hex placeholder in place
    }

    // Defer enqueueing until this node's filters are evaluated, so children of
    // rejected nodes are never loaded.
    pending_children_.push_back({p_child_addr, child_name,
                                 child_options_[child_name],
                                 /*is_root=*/false});
    // Return the raw hex as a placeholder regardless — the engine will
    // patch it to the real child JSON once that node finishes processing.
  }

  return field_json;
}

void ControlBlockNode::enqueuePendingChildren() {
  size_t enqueued = 0;
  for (const PendingNode& child : pending_children_) {
    auto visit_key       = std::make_pair(child.p_address, child.cb_name);
    std::string addr_str = addressToString(child.p_address);
    if (ctx_.visited.find(visit_key) == ctx_.visited.end()) {
      // Mark in-progress before enqueueing so two parents at the same BFS level
      // can't double-enqueue the same child address.
      ctx_.visited[visit_key] = false;  // sentinel
      ctx_.queue.push(child);
      ++enqueued;
      Logger::getInstance().debug("Enqueued '" + child.cb_name +
                                  "' at address " + addr_str);
    } else {
      Logger::getInstance().debug("Already visited/enqueued '" + child.cb_name +
                                  "' at address " + addr_str);
    }
  }
  if (!pending_children_.empty()) {
    Logger::getInstance().debug(
        "'" + cb_.getName() + "' at address " + addressToString(p_address_) +
        ": enqueued " + std::to_string(enqueued) + " of " +
        std::to_string(pending_children_.size()) + " pending child(ren)");
  }
  pending_children_.clear();
}

nlohmann::json ControlBlockNode::parseFields() {
  nlohmann::json cb_json = {};
  std::unordered_map<std::string, control_block_field_t> repeated_fields;
  size_t psa_fields_skipped = 0;

  for (const auto& [field_name, field_data] : cb_.getMap()) {
    if (field_data.end_of_control_block) {
      continue;
    }
    if (cb_.getName() == "psa" &&
        field_data.offset >= (cb_.getMaxOffset() - 8) / 2 &&
        field_data.offset >= 0) {
      ++psa_fields_skipped;
      continue;
    }

    std::string lower_name = field_name;
    std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    if (field_data.repeated) {
      repeated_fields[lower_name] = field_data;
      continue;
    }

    nlohmann::json resolved = ControlBlockNode::fieldToJson(field_data);

    if (resolved.is_object() || resolved.is_array()) {
      // Already an expanded child — preserve raw pointer under "<name>_ptr".
      const char* p_raw =
          static_cast<const char*>(p_address_) + field_data.offset;
      cb_json[lower_name + "_ptr"] =
          (field_data.length == 8)
              ? nlohmann::json(cb_.formatter_.getHex<uint64_t>(p_raw))
              : nlohmann::json(cb_.formatter_.getHex<uint32_t>(p_raw));
    }
    cb_json[lower_name] = resolved;
  }

  if (psa_fields_skipped > 0) {
    Logger::getInstance().debug("Skipped " +
                                std::to_string(psa_fields_skipped) +
                                " 'psa' field(s) beyond the readable range");
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

    size_t entries_ignored = 0;

    for (size_t i = 0; i < count; ++i) {
      // ignoreMask matches (e.g. unused slots) are dropped up front, so they
      // are neither emitted nor followed.  Child filters are applied later by
      // TraversalEngine::resolvePlaceholders.
      if (!field_data.ignore_mask.empty() && !field_data.points_to.empty() &&
          child_options_.find(field_data.points_to) != child_options_.end()) {
        ControlBlockNode::checkDataLength(
            field_data.offset + static_cast<ptrdiff_t>(i) * field_data.length);
        const char* p_entry_raw = static_cast<const char*>(p_address_) +
                                  field_data.offset + i * field_data.length;
        const uint32_t mask = static_cast<uint32_t>(std::stoull(
                                  field_data.ignore_mask, nullptr, 16))
                              << 24;
        if (cb_.formatter_.uint<uint32_t>(p_entry_raw) & mask) {
          ++entries_ignored;
          continue;
        }
      }

      nlohmann::json entry = ControlBlockNode::fieldToJson(
          field_data, static_cast<ptrdiff_t>(i) * field_data.length);

      if (!entry.is_null()) {
        field_json[field_name + "s"].push_back(entry);
      }
    }

    if (entries_ignored > 0) {
      Logger::getInstance().debug(
          "Repeated field '" + field_name + "': ignored " +
          std::to_string(entries_ignored) + " of " + std::to_string(count) +
          " entries (ignoreMask)");
    }

    cb_json.merge_patch(field_json);
  }

  return cb_json;
}

}  // namespace CBXP
