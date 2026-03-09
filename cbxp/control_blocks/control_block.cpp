#include "control_block.hpp"

#include <fnmatch.h>

#include <nlohmann/json.hpp>

#include "control_block_error.hpp"
#include "logger.hpp"

namespace CBXP {
void ControlBlock::createOptionsMap(const std::vector<std::string>& includes,
                                    const std::vector<std::string>& filters) {
  // createFilterLists depends on the construction of the "includes" portion
  // of the options_map_ structure and must be called after createIncludeLists
  ControlBlock::createIncludeLists(includes);
  ControlBlock::createFilterLists(filters);
}

void ControlBlock::createIncludeLists(
    const std::vector<std::string>& includes) {
  Logger::getInstance().debug(
      "Creating include lists for child control blocks to include with the '" +
      control_block_name_ + "' control block...");
  for (std::string include : includes) {
    if (include == "**") {
      Logger::getInstance().debug("Processing '**' include...");
      ControlBlock::processDoubleAsteriskInclude();
      return;
    } else if (include == "*") {
      Logger::getInstance().debug("Processing '*' include...");
      ControlBlock::processAsteriskInclude();
    } else {
      Logger::getInstance().debug("Processing '" + include + "' include...");
      ControlBlock::processExplicitInclude(include);
    }
  }
  Logger::getInstance().debug(
      "Include lists for child control blocks to include with the '" +
      control_block_name_ + "' control block have been created");
}

void ControlBlock::processDoubleAsteriskInclude() {
  // Any existing entries in the hash map are redundant, so clear them
  options_map_.clear();
  for (const std::string& includable : includables_) {
    // Build a map of all includables_ but with "**" at the next level
    Logger::getInstance().debug(
        "Initializing and adding '**' to the include list for the '" +
        includable + "' control block...");
    options_map_[includable].include_patterns = {"**"};
  }
}

void ControlBlock::processAsteriskInclude() {
  if (options_map_.empty()) {
    for (const std::string& includable : includables_) {
      // Build a map of all includables_
      Logger::getInstance().debug("Initializing include list for the '" +
                                  includable + "' control block...");
      options_map_[includable].include_patterns = {};
    }
  }
  for (const std::string& includable : includables_) {
    if (options_map_.find(includable) != options_map_.end()) {
      Logger::getInstance().debug("Include list already exists for the '" +
                                  includable + "' control block");
      continue;
    }
    // Add all includables_ not already present to the map
    Logger::getInstance().debug("Initializing include list for the '" +
                                includable + "' control block...");
    options_map_[includable].include_patterns = {};
  }
}

void ControlBlock::processExplicitInclude(std::string& include) {
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
  if (std::find(includables_.begin(), includables_.end(), include) ==
      includables_.end()) {
    Logger::getInstance().debug(
        "'" + include +
        "' is not a known child control block that can be included with the '" +
        control_block_name_ + "' control block");
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

void ControlBlock::createFilterLists(const std::vector<std::string>& filters) {
  Logger::getInstance().debug(
      "Creating filter lists for the '" + control_block_name_ +
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
      ControlBlock::addCurrentFilter(filter);
    }
  }
  Logger::getInstance().debug(
      "Filter lists for the '" + control_block_name_ +
      "' control block and included child control blocks have been created");
}

void ControlBlock::addCurrentFilter(const std::string& filter) {
  std::string filter_key, filter_value;
  std::vector<std::string> operations = {"<=", ">=", "<", ">", "="};
  for (std::string operation : operations) {
    size_t operation_pos = filter.find(operation);
    if (operation_pos != std::string::npos) {
      // If there's a delimeter then separate include into the key and its value
      filter_value = filter.substr(operation_pos + operation.length());
      filter_key   = filter.substr(0, operation_pos);
      if (filter_value == "\"\"" || filter_value == "\'\'") {
        Logger::getInstance().debug("Filter value converted to null");
        filter_value = "";
      }
      cbxp_filter_t filter_data = {operation, filter_value};
      Logger::getInstance().debug("Adding '" + filter_key + operation +
                                  filter_value +
                                  "' to the current filters list for the '" +
                                  control_block_name_ + "' control block...");
      current_filters_[filter_key].push_back(filter_data);
      return;
    }
  }
}

bool ControlBlock::compare(const nlohmann::json& json_value,
                           const std::string& filter_value,
                           const std::string& operation) {
  std::string value_str = "";
  bool value_is_string  = false;
  uint64_t value_uint;

  if (json_value.is_number()) {
    value_uint = json_value.get<int>();
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

bool ControlBlock::matchFilter(nlohmann::json& control_block_json) {
  Logger::getInstance().debug("Applying filters to the '" +
                              control_block_name_ + "' control block...");
  if (current_filters_.empty()) {
    // If the filter map is empty then we want to return the control block
    Logger::getInstance().debug("No filters were provided for the '" +
                                control_block_name_ + "' control block");
    return true;
  }
  for (const auto& [filter_key, filter_list] : current_filters_) {
    if (!control_block_json.contains(filter_key)) {
      Logger::getInstance().debug(
          "The filter key '" + filter_key +
          "' does not correspond to any control block field in the '" +
          control_block_name_ + "' control block");
      throw FilterError();
    }
    // cppcheck-suppress useStlAlgorithm
    for (const cbxp_filter_t& filter_data : filter_list) {
      // would require capturing structured bindings to use all_of or none_of
      Logger::getInstance().debug("Applying filter '" + filter_key +
                                  filter_data.operation + filter_data.value +
                                  "'...");
      if (!ControlBlock::compare(control_block_json[filter_key],
                                 filter_data.value, filter_data.operation)) {
        Logger::getInstance().debug("The filter '" + filter_key +
                                    filter_data.operation + filter_data.value +
                                    "' did not match");
        return false;
      }
    }
  }
  // If we didn't have a reason to return false, we return true
  Logger::getInstance().debug("All filters for the '" + control_block_name_ +
                              "' control block matched");
  return true;
}
}  // namespace CBXP

