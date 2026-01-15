#ifndef __CONTROL_BLOCK_H_
#define __CONTROL_BLOCK_H_

#include <nlohmann/json.hpp>

#include "control_block_field_formatter.hpp"

namespace CBXP {

typedef struct {
  std::string operation;
  std::string value;
} cbxp_filter_t;

typedef struct {
  std::vector<std::string> include_patterns;
  std::vector<std::string> filters;
} cbxp_options_t;

class ControlBlock {
 private:
  const std::string control_block_name_;
  const std::vector<std::string> includables_;
  void processDoubleAsteriskInclude();
  void processAsteriskInclude();
  void processExplicitInclude(std::string& include);
  void addCurrentFilter(const std::string& filter);
  void createIncludeMap(const std::vector<std::string>& includes);
  void createFilterMap(const std::vector<std::string>& filters);
  bool compare(const nlohmann::json& json_value,
               const std::string& filter_value, const std::string& operation);

 protected:
  ControlBlockFieldFormatter formatter_;
  std::unordered_map<std::string, cbxp_options_t> options_map_;
  std::unordered_map<std::string, std::vector<cbxp_filter_t>> current_filters_;
  bool matchFilter(nlohmann::json& control_block_json);
  void createOptionsMap(const std::vector<std::string>& includes,
                        const std::vector<std::string>& filters);

 public:
  virtual nlohmann::json get(void* __ptr32 p_control_block = nullptr) = 0;
  explicit ControlBlock(const std::string& name,
                        const std::vector<std::string>& includables,
                        const cbxp_options_t& cbxp_options)
      : control_block_name_(name), includables_(includables) {
    createOptionsMap(cbxp_options.include_patterns, cbxp_options.filters);
  }
  virtual ~ControlBlock() = default;
};

}  // namespace CBXP

#endif
