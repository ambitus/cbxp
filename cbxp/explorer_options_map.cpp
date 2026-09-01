#include "explorer_options_map.hpp"

#include <fnmatch.h>

#include <algorithm>
#include <cctype>
#include <nlohmann/json.hpp>
#include <sstream>

#include "control_block_error.hpp"
#include "logger.hpp"

namespace CBXP {

void ExplorerOptionsMap::createIncludeLists(
    const std::vector<std::string>& includes) {
  Logger::getInstance().debug(
      "Creating include lists for child control blocks to include with the '" +
      control_block_->getName() + "' control block...");
  for (std::string include : includes) {
    if (include == "**") {
      Logger::getInstance().debug("Processing '**' include...");
      ExplorerOptionsMap::processDoubleAsteriskInclude();
      return;
    } else if (include == "*") {
      Logger::getInstance().debug("Processing '*' include...");
      ExplorerOptionsMap::processAsteriskInclude();
    } else {
      Logger::getInstance().debug("Processing '" + include + "' include...");
      ExplorerOptionsMap::processExplicitInclude(include);
    }
  }
  Logger::getInstance().debug(
      "Include lists for child control blocks to include with the '" +
      control_block_->getName() + "' control block have been created");
}

void ExplorerOptionsMap::processDoubleAsteriskInclude() {
  // Any existing entries in the hash map are redundant, so clear them
  options_map_.clear();
  for (const std::string& includable : control_block_->getIncludables()) {
    // Build a map of all control_block_details_.includables but with "**" at
    // the next level
    Logger::getInstance().debug(
        "Initializing and adding '**' to the include list for the '" +
        includable + "' control block...");
    options_map_[includable].include_patterns = {"**"};
  }
}

void ExplorerOptionsMap::processAsteriskInclude() {
  if (options_map_.empty()) {
    for (const std::string& includable : control_block_->getIncludables()) {
      // Build a map of all control_block_details_.includables
      Logger::getInstance().debug("Initializing include list for the '" +
                                  includable + "' control block...");
      options_map_[includable].include_patterns = {};
    }
  }
  for (const std::string& includable : control_block_->getIncludables()) {
    if (options_map_.find(includable) != options_map_.end()) {
      Logger::getInstance().debug("Include list already exists for the '" +
                                  includable + "' control block");
      continue;
    }
    // Add all control_block_details_.includables not already present to the map
    Logger::getInstance().debug("Initializing include list for the '" +
                                includable + "' control block...");
    options_map_[includable].include_patterns = {};
  }
}

void ExplorerOptionsMap::processExplicitInclude(std::string& include) {
  // Default case; have to validate against an includable
  const std::string del        = ".";
  std::string include_includes = "";
  size_t del_pos               = include.find(del);
  if (del_pos != std::string::npos) {
    // If there's a "." then separate include into the include and its
    // includes
    include_includes = include.substr(del_pos + 1);
    include.resize(del_pos);
  }
  if (std::find(control_block_->getIncludables().begin(),
                control_block_->getIncludables().end(),
                include) == control_block_->getIncludables().end()) {
    Logger::getInstance().debug(
        "'" + include +
        "' is not a known child control block that can be included with the '" +
        control_block_->getName() + "' control block");
    throw IncludeError();
  }
  if (options_map_.find(include) == options_map_.end()) {
    // If we don't already have this include in our map, add it with its
    // includes
    if (include_includes == "") {
      Logger::getInstance().debug("Initializing include list for the '" +
                                  include + "' control block...");
      options_map_[include].include_patterns = {};
    } else {
      Logger::getInstance().debug("Adding '" + include_includes +
                                  "' to the include list for the '" + include +
                                  "' control block...");
      options_map_[include].include_patterns = {include_includes};
    }
  } else {
    // If we DO already have this in our map, then we should add its
    // includes if they are useful or new
    if (std::find(options_map_[include].include_patterns.begin(),
                  options_map_[include].include_patterns.end(),
                  include_includes) !=
        options_map_[include].include_patterns.end()) {
      return;
    }
    if (include_includes == "") {
      return;
    }
    Logger::getInstance().debug("Adding '" + include_includes +
                                "' to the include list for the '" + include +
                                "' control block...");
    options_map_[include].include_patterns.push_back(include_includes);
  }
}

void ExplorerOptionsMap::createFilterLists(
    const std::vector<std::string>& filters) {
  Logger::getInstance().debug(
      "Creating filter lists for the '" + control_block_->getName() +
      "' control block and included child control blocks...");
  for (const std::string& filter : filters) {
    // Only case; specific non-generic filter
    const std::string del = ".";
    size_t del_pos        = filter.find(del);
    if (del_pos != std::string::npos) {
      // If there's a "." then separate filter into the control_block
      // and its filter
      std::string control_block_filter = filter.substr(del_pos + 1);
      std::string control_block        = filter.substr(0, del_pos);

      // Check to make sure we are including the specified control block
      auto it = options_map_.find(control_block);
      if (it == options_map_.end()) {
        Logger::getInstance().debug(
            "A filter that requires the '" + control_block +
            "' control block was provided, but the '" + control_block +
            "' control block was not included");
        throw FilterError();
      }
      Logger::getInstance().debug("Adding '" + control_block_filter +
                                  "' to the filter list for the '" +
                                  control_block + "' control block...");
      options_map_[control_block].filters.push_back(control_block_filter);
    } else {
      ExplorerOptionsMap::addCurrentFilter(filter);
    }
  }
  Logger::getInstance().debug(
      "Filter lists for the '" + control_block_->getName() +
      "' control block and included child control blocks have been created");
}

void ExplorerOptionsMap::addCurrentFilter(const std::string& filter) {
  std::vector<std::string> operations = {"<=", ">=", "<", ">", "="};
  for (std::string operation : operations) {
    size_t operation_pos = filter.find(operation);
    if (operation_pos != std::string::npos) {
      // If there's a delimeter then separate include into the key and its value
      std::string filter_value =
          filter.substr(operation_pos + operation.length());
      std::string filter_key    = filter.substr(0, operation_pos);
      cbxp_filter_t filter_data = {operation, filter_value};
      Logger::getInstance().debug(
          "Adding '" + filter_key + operation + filter_value +
          "' to the current filters list for the '" +
          control_block_->getName() + "' control block...");
      current_filters_[filter_key].push_back(filter_data);
      return;
    }
  }
  Logger::getInstance().debug(
      "Filters must be key-value pairs (e.g., 'key=value')");
  throw FilterError();
}

bool ExplorerOptionsMap::compare(const nlohmann::json& json_value,
                                 const std::string& filter_value,
                                 const std::string& operation) {
  std::string value_str = "";
  bool value_is_string  = false;
  uint64_t value_uint;

  if (json_value.is_number()) {
    value_uint = json_value.get<uint64_t>();
  } else {
    value_str       = json_value.get<std::string>();
    value_is_string = true;
    if (value_str.substr(0, 2) == "0x") {
      value_uint      = std::stoull(value_str, nullptr, 0);
      value_is_string = false;
    }
  }
  if (value_is_string) {
    // Filter is testing strings
    if (operation == "=") {
      Logger::getInstance().debug("\"" + value_str + "\" = \"" + filter_value +
                                  "\" ?");
      return (fnmatch(filter_value.c_str(), value_str.c_str(), 0) == 0);
    } else {
      Logger::getInstance().debug(
          "<, <=, >, and >= cannot be used with string filter values");
      throw FilterError();
    }
  }  // Filter is testing non-strings
  else {
    uint64_t filter_uint;
    try {
      filter_uint = std::stoull(filter_value, nullptr, 0);
    } catch (...) {
      Logger::getInstance().debug("'" + filter_value +
                                  "' cannot be compared to a numeric value");
      throw FilterError();
    }
    Logger::getInstance().debug(std::to_string(value_uint) + " " + operation +
                                " " + std::to_string(filter_uint) + " ?");
    if (operation == "=") {
      return value_uint == filter_uint;
    } else if (operation == ">") {
      return value_uint > filter_uint;
    } else if (operation == "<") {
      return value_uint < filter_uint;
    } else if (operation == ">=") {
      return value_uint >= filter_uint;
    } else if (operation == "<=") {
      return value_uint <= filter_uint;
    }
  }
  // We should never get here, so it would be good to say "no match" just in
  // case
  return false;
}

bool ExplorerOptionsMap::matchFilter(nlohmann::json& control_block_json) {
  Logger::getInstance().debug("Applying filters to the '" +
                              control_block_->getName() + "' control block...");
  if (current_filters_.empty()) {
    // If the filter map is empty then we want to return the control block
    Logger::getInstance().debug("No filters were provided for the '" +
                                control_block_->getName() + "' control block");
    return true;
  }
  for (const auto& [filter_key, filter_list] : current_filters_) {
    if (!control_block_json.contains(filter_key)) {
      Logger::getInstance().debug(
          "The filter key '" + filter_key +
          "' does not correspond to any control block field in the '" +
          control_block_->getName() + "' control block");
      throw FilterError();
    }
    // cppcheck-suppress useStlAlgorithm
    for (const cbxp_filter_t& filter_data : filter_list) {
      // would require capturing structured bindings to use all_of or none_of
      Logger::getInstance().debug("Applying filter '" + filter_key +
                                  filter_data.operation + filter_data.value +
                                  "'...");
      if (!ExplorerOptionsMap::compare(control_block_json[filter_key],
                                       filter_data.value,
                                       filter_data.operation)) {
        Logger::getInstance().debug("The filter '" + filter_key +
                                    filter_data.operation + filter_data.value +
                                    "' did not match");
        return false;
      }
    }
  }
  // If we didn't have a reason to return false, we return true
  Logger::getInstance().debug("All filters for the '" +
                              control_block_->getName() +
                              "' control block matched");
  return true;
}

nlohmann::json ExplorerOptionsMap::fieldToJson(control_block_field_t field_data,
                                               ptrdiff_t offset) {
  offset += field_data.offset;
  ExplorerOptionsMap::checkDataLength(offset);
  nlohmann::json field_json;
  // do try/except to stop an 0C4?
  // check points to field against any current inclusions, if so call this
  // recursively for that control block
  try {
    switch (field_data.type) {
      case STRING:
        field_json = control_block_->formatter_.getString(
            static_cast<const char*>(p_control_block_) + offset,
            field_data.length);
        break;
      case UNSIGNED_INT:
        if (field_data.length == 8) {
          field_json = control_block_->formatter_.uint<uint64_t>(
              static_cast<const char*>(p_control_block_) + offset);
        } else {
          field_json = control_block_->formatter_.uint<uint32_t>(
              static_cast<const char*>(p_control_block_) + offset);
        }
        break;
      case SIGNED_INT:
        if (field_data.length == 8) {
          field_json = *(reinterpret_cast<const int64_t*>(
              static_cast<const char*>(p_control_block_) + offset));
        } else if (field_data.length == 2) {
          field_json = *(reinterpret_cast<const int16_t*>(
              static_cast<const char*>(p_control_block_) + offset));
        } else {
          field_json = *(reinterpret_cast<const int32_t*>(
              static_cast<const char*>(p_control_block_) + offset));
        }
        break;
      case BITSTRING:
        // We can later differentiate this from Hex
        if (field_data.length == 8) {
          field_json = control_block_->formatter_.getHex<uint64_t>(
              static_cast<const char*>(p_control_block_) + offset);
        } else {
          field_json = control_block_->formatter_.getHex<uint32_t>(
              static_cast<const char*>(p_control_block_) + offset);
        }
        break;
      case HEX:
        if (field_data.length == 8) {
          field_json = control_block_->formatter_.getHex<uint64_t>(
              static_cast<const char*>(p_control_block_) + offset);
        } else {
          field_json = control_block_->formatter_.getHex<uint32_t>(
              static_cast<const char*>(p_control_block_) + offset);
        }
        break;
      case ADDRESS:
        if (field_data.length == 8) {
          field_json = control_block_->formatter_.getHex<uint64_t>(
              static_cast<const char*>(p_control_block_) + offset);
        } else {
          field_json = control_block_->formatter_.getHex<uint32_t>(
              static_cast<const char*>(p_control_block_) + offset);
        }
        break;
    }
  } catch (const std::exception& e) {
    Logger::getInstance().debug("Unexpected error occurred.");
    throw CbxpReachBlockError();
  }

  if (!field_data.pointsTo.empty() &&
      options_map_.find(field_data.pointsTo) != options_map_.end()) {
    std::string control_block_name = field_data.pointsTo;
    const void* p_control_block;
    if (field_data.length == 8) {
      p_control_block =
          reinterpret_cast<const void*>(field_json.get<uint64_t>());
    } else {
      p_control_block =
          reinterpret_cast<const void* __ptr32>(field_json.get<uint32_t>());
    }
    field_json =
        ExplorerOptionsMap(options_map_[control_block_name], control_block_name,
                           control_blocks_, p_control_block, 0)
            .getControlBlockData(true);
  }
  return field_json;
}

void ExplorerOptionsMap::checkDataLength(const ptrdiff_t offset) const {
  if (skip_buffer_length_check_) {
    // Data length check is only done when formatting
    // user provided control block data.
    // This check is skipped when extracting and formatting
    // control block data from live memory.
    return;
  }
  // Negative offsets are valid (e.g. CVT fields before the nominal base
  // pointer) and are always within range; no buffer length check needed.
  if (offset < 0) {
    return;
  }
  Logger::getInstance().debug(
      "Checking if specified buffer (" + std::to_string(buffer_length_) +
      " bytes) too small to contain the '" + control_block_->getName() +
      "' control block (requires at least " + std::to_string(offset) +
      " bytes)...");
  if (static_cast<size_t>(offset) > buffer_length_) {
    throw DataLengthError();
  }
}

const std::vector<const void*> ExplorerOptionsMap::findControlBlockPointer(
    std::optional<ControlBlock> control_block) const {
  // We have to go exploring for the control block we want
  //  1 - Find current control block
  //  2 - Check for pointed to by, if empty, check fixed address, if have that,
  //  move on Loop through pointed to by, checking next pointed to by, then
  //  looking for fixed address, until hit fixed address (log steps) Start from
  //  fixed address, iterate through fields looking for points to, keep going
  //  until we hit our source control block do try except to stop an 0C4 Set our
  //  p_control_block value
  std::vector<const void*> control_block_pointers{};
  std::vector<std::string> pointed_to_by = control_block->getPointedToBy();
  if (pointed_to_by.empty()) {
    return {control_block->getFixedAddress()};
  }
  for (const auto& pointer_name : pointed_to_by) {
    // we may eventually want to highlight a specific path rather than choosing
    // "all possible paths"
    std::optional<ControlBlock> next_control_block =
        control_blocks_.at(pointer_name);
    std::unordered_map<std::string, control_block_field_t>
        next_control_block_map = next_control_block->getMap();
    const std::vector<const void*> next_vector =
        ExplorerOptionsMap::findControlBlockPointer(next_control_block);
    std::string count_field        = "";
    std::string pointer_field_name = "";
    ptrdiff_t offset               = 0;
    size_t field_length            = 4;
    for (const auto& [field_name, field_data] : next_control_block_map) {
      if (field_data.pointsTo.empty() ||
          field_data.pointsTo != control_block->getName()) {
        continue;
      }
      offset             = field_data.offset;
      field_length       = field_data.length;
      count_field        = field_data.count;
      pointer_field_name = field_name;
      break;
    }
    for (const auto& next : next_vector) {
      // In case we are ALREADY looking at a list of control blocks, we iterate
      // through this
      Logger::getInstance().debug("Polling '" + next_control_block->getName() +
                                  "' field '" + pointer_field_name +
                                  "' at offset '" + std::to_string(offset) +
                                  "' for control block tree...");
      size_t count = 1;
      if (!count_field.empty()) {
        count = *(reinterpret_cast<const size_t*>(
            static_cast<const char*>(next) +
            next_control_block_map[count_field].offset));
      }
      for (size_t i = 0; i < count; i++) {
        // Then we iterate through the "next" control blocks
        const char* base =
            static_cast<const char*>(next) + i * field_length + offset;
        const void* resolved;
        if (field_length == 8) {
          resolved = reinterpret_cast<const void*>(
              *reinterpret_cast<const uint64_t*>(base));
        } else {
          // __ptr32 is a z/OS platform qualifier; branches are intentionally
          // distinct
          resolved = reinterpret_cast<const void* __ptr32>(
              *reinterpret_cast<const uint32_t*>(base));
        }
        Logger::getInstance().debug("Resolved '" + control_block->getName() +
                                    "' pointer to address " + [&]() {
                                      std::ostringstream oss;
                                      oss << resolved;
                                      return oss.str();
                                    }());
        control_block_pointers.push_back(resolved);
      }
    }
  }
  return control_block_pointers;
}

nlohmann::json ExplorerOptionsMap::parseFields() {
  nlohmann::json control_block_data = {};
  std::unordered_map<std::string, control_block_field_t> repeated_fields;
  for (const auto& [field_name, field_data] : control_block_->getMap()) {
    nlohmann::json field_json = {};
    if (control_block_->getName() == "psa" &&
        field_data.offset >= (control_block_->getMaxOffset() - 8) / 2 &&
        field_data.offset >= 0) {
      // Half of the PSA is fetch protected, so we can't actually get the "back
      // half" of it
      continue;
    }
    Logger::getInstance().debug("Loading field '" + field_name +
                                "' at offset '" +
                                std::to_string(field_data.offset) + "'");
    if (field_data.repeated) {
      repeated_fields[field_name] = field_data;
      continue;
    } else {
      field_json[field_name] = ExplorerOptionsMap::fieldToJson(field_data);
    }
    control_block_data.merge_patch(field_json);
  }
  for (const auto& [field_name, field_data] : repeated_fields) {
    field_json[field_name + "s"] = {};
    // Transform the string in place
    std::transform(field_data.count.begin(), field_data.count.end(),
                   field_data.count.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    size_t count = control_block_data[field_data.count].get<uint8_t>();
    Logger::getInstance().debug("Looping through '" + std::to_string(count) +
                                "' '" + field_name + "'s");
    for (int i = 1; i <= count; i++) {
      field_json[field_name + "s"].push_back(
          ExplorerOptionsMap::fieldToJson(field_data, (i * field_data.length)));
    }
  }
  return control_block_data;
}

nlohmann::json ExplorerOptionsMap::getControlBlockData(const bool recursive) {
  std::string control_block_name = control_block_->getName();
  std::vector<const void*> p_control_blocks;
  nlohmann::json control_block_data = {}, json_to_return = {};
  ;
  if (p_control_block_ == nullptr or recursive == true) {
    // If we look for a control block in live memory, we need to check that we
    // can reach it If the call is recursive, we WILL have a target address
    // already
    const storage_attributes_t attributes =
        control_block_->getStorageAttributes();
    if (attributes.key != 8 and attributes.is_protected and
        control_block_name != "psa") {
      // The PSA only HALF exists in fetch protected storage, so it gets a
      // hard-coded exception
      Logger::getInstance().debug(
          "The '" + control_block_name + "' control block is key '" +
          std::to_string(attributes.key) +
          "' and fetch protected, so cbxp cannot access it.");
      throw CbxpReachBlockError();
    }
    if (!attributes.is_common and current_filters_.empty()) {
      Logger::getInstance().debug("The '" + control_block_name +
                                  "' control block is not in common storage " +
                                  " and there are no filters enabled, so cbxp "
                                  "cannot guarantee accuracy.");
      throw CbxpReachBlockError();
    }
  }
  if (p_control_block_ == nullptr && recursive == false) {
    p_control_blocks =
        ExplorerOptionsMap::findControlBlockPointer(control_block_);
  }
  if (p_control_blocks.size() == 0) {
    Logger::getInstance().debug("No pointers found for '" + control_block_name +
                                "' control block.");
    return control_block_data;
  } else if (p_control_blocks.size() == 1) {
    p_control_block_ = p_control_blocks[0];
    int dump_length  = control_block_name == "psa"
                           ? (control_block_->getMaxOffset() - 8) / 2
                           : control_block_->getMaxOffset();
    Logger::getInstance().debug(control_block_name + " hex dump: address=" +
                                [&]() {
                                  std::ostringstream oss;
                                  oss << p_control_block_;
                                  return oss.str();
                                }() +
                                " length=" + std::to_string(dump_length));
    Logger::getInstance().hexDump(
        reinterpret_cast<const char*>(p_control_block_), dump_length,
        control_block_name == "psa");
    nlohmann::json single_control_block_json =
        ExplorerOptionsMap::parseFields();
    if (ExplorerOptionsMap::matchFilter(single_control_block_json)) {
      control_block_data[control_block_name] = single_control_block_json;
    }
  } else {
    control_block_data[control_block_name + "s"] = {};
    for (const auto& p_control_block : p_control_blocks) {
      p_control_block_ = p_control_block;
      Logger::getInstance().debug(
          control_block_->getName() + " hex dump: address=" +
          [&]() {
            std::ostringstream oss;
            oss << p_control_block_;
            return oss.str();
          }() +
          " length=" + std::to_string(control_block_->getMaxOffset()));
      Logger::getInstance().hexDump(
          reinterpret_cast<const char*>(p_control_block_),
          control_block_->getMaxOffset());
      nlohmann::json single_control_block_json =
          ExplorerOptionsMap::parseFields();
      if (ExplorerOptionsMap::matchFilter(single_control_block_json)) {
        control_block_data[control_block_name + "s"].push_back(
            single_control_block_json);
      }
    }
  }
  return control_block_data;
}

}  // namespace CBXP

