#ifndef __CBXP_FILTER_SET_H_
#define __CBXP_FILTER_SET_H_

#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "cbxp_types.hpp"

namespace CBXP {

typedef struct {
  std::string operation;
  std::string value;
} cbxp_filter_t;

// Parses a filter-string list and evaluates them against a JSON object.
class FilterSet {
 public:
  // Parses `filters`; those targeting child blocks ("ascb.field=value") go into
  // `child_options` keyed by child name.
  void parse(const std::vector<std::string>& filters,
             std::unordered_map<std::string, cbxp_options_t>& child_options,
             const std::string& cb_name);

  // Returns true if `cb_json` satisfies every filter in this set.
  bool match(const nlohmann::json& cb_json) const;

 private:
  // field_name → list of (operation, value) pairs
  std::unordered_map<std::string, std::vector<cbxp_filter_t>> filters_;

  void addFilter(const std::string& filter, const std::string& cb_name);

  static bool compare(const nlohmann::json& json_value,
                      const std::string& filter_value,
                      const std::string& operation);
};

}  // namespace CBXP

#endif
