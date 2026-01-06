#ifndef __CONTROL_BLOCK_H_
#define __CONTROL_BLOCK_H_

#include <nlohmann/json.hpp>

#include "control_block_field_formatter.hpp"

namespace CBXP {

class ControlBlock {
 private:
  const std::string control_block_name_;
  const std::vector<std::string> includables_;
  void processDoubleAsteriskInclude();
  void processAsteriskInclude();
  void processExplicitInclude(std::string& include);
  void processFilterValue(const std::string& filter);
  bool testValue(const nlohmann::json& json_value,
                 const std::string& filter_value, const std::string& operation);

 protected:
  ControlBlockFieldFormatter formatter_;
  std::unordered_map<std::string, std::vector<std::string>> include_map_;
  std::unordered_map<std::string, std::vector<std::string>> filter_map_;
  std::unordered_map<std::string, std::vector<std::string>> current_filters_;
  bool matchFilter(nlohmann::json& control_block_json);

 public:
  void createIncludeMap(const std::vector<std::string>& includes);
  void createFilterMap(const std::vector<std::string>& filters);
  virtual nlohmann::json get(void* __ptr32 p_control_block = nullptr) = 0;
  explicit ControlBlock(const std::string& name,
                        const std::vector<std::string>& includables,
                        const std::vector<std::string>& includes,
                        const std::vector<std::string>& filters)
      : control_block_name_(name), includables_(includables) {
    createIncludeMap(includes);
    createFilterMap(filters);
  }
  virtual ~ControlBlock() = default;
};

}  // namespace CBXP

#endif
