#include "filter_set.hpp"

#include <fnmatch.h>

#include <vector>

#include "control_block_error.hpp"
#include "logger.hpp"

namespace CBXP {

void FilterSet::parse(
    const std::vector<std::string>& filters,
    std::unordered_map<std::string, cbxp_options_t>& child_options,
    const std::string& cb_name) {
  Logger::getInstance().debug(
      "Creating filter lists for the '" + cb_name +
      "' control block and included child control blocks...");

  for (const std::string& filter : filters) {
    const std::string del = ".";
    size_t del_pos        = filter.find(del);
    if (del_pos != std::string::npos) {
      std::string child_cb     = filter.substr(0, del_pos);
      std::string child_filter = filter.substr(del_pos + 1);
      auto it                  = child_options.find(child_cb);
      if (it == child_options.end()) {
        Logger::getInstance().debug("A filter that requires the '" + child_cb +
                                    "' control block was provided, but the '" +
                                    child_cb +
                                    "' control block was not included");
        throw FilterError();
      }
      Logger::getInstance().debug("Adding '" + child_filter +
                                  "' to the filter list for the '" + child_cb +
                                  "' control block...");
      it->second.filters.push_back(child_filter);
    } else {
      addFilter(filter, cb_name);
    }
  }

  Logger::getInstance().debug(
      "Filter lists for the '" + cb_name +
      "' control block and included child control blocks have been created");
}

void FilterSet::addFilter(const std::string& filter,
                          const std::string& cb_name) {
  static const std::vector<std::string> operations = {"<=", ">=", "<", ">",
                                                      "="};
  for (const std::string& operation : operations) {
    size_t op_pos = filter.find(operation);
    if (op_pos != std::string::npos) {
      std::string filter_key    = filter.substr(0, op_pos);
      std::string filter_value  = filter.substr(op_pos + operation.length());
      cbxp_filter_t filter_data = {operation, filter_value};
      Logger::getInstance().debug("Adding '" + filter_key + operation +
                                  filter_value +
                                  "' to the current filters list for the '" +
                                  cb_name + "' control block...");
      filters_[filter_key].push_back(filter_data);
      return;
    }
  }
  Logger::getInstance().debug(
      "Filters must be key-value pairs (e.g., 'key=value')");
  throw FilterError();
}

bool FilterSet::compare(const nlohmann::json& json_value,
                        const std::string& filter_value,
                        const std::string& operation) {
  std::string value_str = "";
  bool value_is_string  = false;
  uint64_t value_uint   = 0;

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
    if (operation == "=") {
      Logger::getInstance().debug("\"" + value_str + "\" = \"" + filter_value +
                                  "\" ?");
      return (fnmatch(filter_value.c_str(), value_str.c_str(), 0) == 0);
    } else {
      Logger::getInstance().debug(
          "<, <=, >, and >= cannot be used with string filter values");
      throw FilterError();
    }
  }

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
  if (operation == "=") return value_uint == filter_uint;
  if (operation == ">") return value_uint > filter_uint;
  if (operation == "<") return value_uint < filter_uint;
  if (operation == ">=") return value_uint >= filter_uint;
  if (operation == "<=") return value_uint <= filter_uint;
  return false;
}

bool FilterSet::match(const nlohmann::json& cb_json,
                      bool child_filter_failed) const {
  if (child_filter_failed) {
    return false;
  }
  if (filters_.empty()) {
    return true;
  }
  for (const auto& [filter_key, filter_list] : filters_) {
    if (!cb_json.contains(filter_key)) {
      Logger::getInstance().debug(
          "The filter key '" + filter_key +
          "' does not correspond to any control block field");
      throw FilterError();
    }
    const nlohmann::json& json_field = cb_json[filter_key];
    const std::string ptr_key        = filter_key + "_ptr";
    const nlohmann::json& effective_value =
        (json_field.is_object() || json_field.is_array()) &&
                cb_json.contains(ptr_key)
            ? cb_json[ptr_key]
            : json_field;
    // cppcheck-suppress useStlAlgorithm; loop body contains throw and
    // debug logging; std::all_of cannot express this pattern.
    for (const cbxp_filter_t& filter_data : filter_list) {
      Logger::getInstance().debug("Applying filter '" + filter_key +
                                  filter_data.operation + filter_data.value +
                                  "'...");
      if (!compare(effective_value, filter_data.value, filter_data.operation)) {
        Logger::getInstance().debug("The filter '" + filter_key +
                                    filter_data.operation + filter_data.value +
                                    "' did not match");
        return false;
      }
    }
  }
  return true;
}

}  // namespace CBXP
