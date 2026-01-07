#include "control_block.hpp"

#include <fnmatch.h>

#include <nlohmann/json.hpp>

#include "control_block_error.hpp"
#include "logger.hpp"

namespace CBXP {
void ControlBlock::createFilterMap(const std::vector<std::string>& filters) {
  Logger::getInstance().debug("Creating filter map for the '" +
                              control_block_name_ + "' control block...");
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
      bool control_block_in_inclusion = false;
      for (const auto& [include, include_includes] : include_map_) {
        if (control_block == include) {
          control_block_in_inclusion = true;
        }
      }
      if (!control_block_in_inclusion) {
        Logger::getInstance().debug("'" + control_block +
                                    "' is not specified in the inclusion list");
        throw FilterError();
      }
      filter_map_[control_block].push_back(control_block_filter);
    } else {
      ControlBlock::processFilterValue(filter);
    }
  }
  Logger::getInstance().debug("Done");
}

void ControlBlock::processFilterValue(const std::string& filter) {
  std::string filter_key, filter_value;
  std::vector<std::string> delimeters = {"<=", ">=", "<", ">", "="};
  for (std::string del : delimeters) {
    size_t del_pos = filter.find(del);
    if (del_pos != std::string::npos) {
      // If there's a delimeter then separate include into the key and its value
      filter_value = filter.substr(del_pos + del.length());
      filter_key   = filter.substr(0, del_pos);
      current_filters_[filter_key].push_back(filter_value);
      current_filters_[filter_key].push_back(del);
      return;
    }
  }
}

bool ControlBlock::testValue(const nlohmann::json& json_value,
                             const std::string& filter_value,
                             const std::string& operation) {
  std::string value_str;
  try {
    try {
      int value_int  = json_value.get<int>();
      int filter_int = std::stoi(filter_value);
      if (operation == "=") {
        return value_int == filter_int;
      } else if (operation == ">") {
        return value_int > filter_int;
      } else if (operation == "<") {
        return value_int < filter_int;
      } else if (operation == ">=") {
        return value_int >= filter_int;
      } else if (operation == "<=") {
        return value_int <= filter_int;
      }
    } catch (...) {
      value_str = json_value.get<std::string>();
      if (value_str.substr(0, 2) == "0x") {
        unsigned long value_ulong  = std::stoul(value_str.substr(2));
        unsigned long filter_ulong = std::stoul(filter_value);
        if (operation == "=") {
          return value_ulong == filter_ulong;
        } else if (operation == ">") {
          return value_ulong > filter_ulong;
        } else if (operation == "<") {
          return value_ulong < filter_ulong;
        } else if (operation == ">=") {
          return value_ulong >= filter_ulong;
        } else if (operation == "<=") {
          return value_ulong <= filter_ulong;
        }
      }
    }
    if (operation == "=") {
      size_t last_non_space = value_str.find_last_not_of(" \t\n\r\f\v");
      value_str.resize(last_non_space + 1);
      std::transform(value_str.begin(), value_str.end(), value_str.begin(),
                     [](unsigned char c) { return std::tolower(c); });
      return (fnmatch(filter_value.c_str(), value_str.c_str(), 0) == 0);
    } else {
      Logger::getInstance().debug("Cannot use <,<=,> or >= with string filter");
      throw FilterError();
    }
  } catch (...) {
    Logger::getInstance().debug(
        "Error with type conversions for filter evaluation");
    throw FilterError();
  }
}

bool ControlBlock::matchFilter(nlohmann::json& control_block_json) {
  for (const auto& [json_key, json_value] : control_block_json.items()) {
    if (json_value.is_null()) {
      // If any element in our json structure is null, we already failed a
      // filter match
      return false;
    }
  }
  if (current_filters_.empty()) {
    // If the filter map is empty then we want to return the control block
    return true;
  }
  for (const auto& [filter_key, filter_values] : current_filters_) {
    if (control_block_json.contains(filter_key)) {
      for (size_t i = 0; i < filter_values.size(); i += 2) {
        const std::string& filter_value = filter_values[i];
        const std::string& operation    = filter_values[i + 1];
        if (!ControlBlock::testValue(control_block_json[filter_key],
                                     filter_value, operation)) {
          return false;
        }
      }
    } else {
      Logger::getInstance().debug(
          "Specified filter key not found in control block json");
      throw FilterError();
    }
  }
  // If we didn't have a reason to return false, we return true
  return true;
}

void ControlBlock::createIncludeMap(const std::vector<std::string>& includes) {
  Logger::getInstance().debug("Creating include map for the '" +
                              control_block_name_ + "' control block...");
  for (std::string include : includes) {
    if (include == "**") {
      ControlBlock::processDoubleAsteriskInclude();
      return;
    } else if (include == "*") {
      ControlBlock::processAsteriskInclude();
    } else {
      ControlBlock::processExplicitInclude(include);
    }
  }
  Logger::getInstance().debug("Done");
}

void ControlBlock::processDoubleAsteriskInclude() {
  // Any existing entries in the hash map are redundant, so clear them
  include_map_.clear();
  for (const std::string& includable : includables_) {
    // Build a map of all includables_ but with "**" at the next level
    include_map_[includable] = {"**"};
  }
}

void ControlBlock::processAsteriskInclude() {
  if (include_map_.empty()) {
    for (const std::string& includable : includables_) {
      // Build a map of all includables_
      include_map_[includable] = {};
    }
  }
  for (const std::string& includable : includables_) {
    if (include_map_.find(includable) != include_map_.end()) {
      continue;
    }
    // Add all includables_ not already present to the map
    include_map_[includable] = {};
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
    Logger::getInstance().debug("'" + include +
                                "' is not a known includable for the '" +
                                control_block_name_ + "' control block");
    throw IncludeError();
  }
  if (include_map_.find(include) == include_map_.end()) {
    // If we don't already have this include in our map, add it with its
    // includes
    if (include_includes == "") {
      include_map_[include] = {};
    } else {
      include_map_[include] = {include_includes};
    }
  } else {
    // If we DO already have this in our map, then we should add its
    // includes if they are useful or new
    if (std::find(include_map_[include].begin(), include_map_[include].end(),
                  include_includes) != include_map_[include].end()) {
      return;
    }
    if (include_includes == "") {
      return;
    }
    include_map_[include].push_back(include_includes);
  }
}
}  // namespace CBXP

