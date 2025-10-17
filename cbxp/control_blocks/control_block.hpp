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

 protected:
  ControlBlockFieldFormatter formatter_;
  std::unordered_map<std::string, std::vector<std::string>> include_map_;

 public:
  void createIncludeMap(const std::vector<std::string>& includes);
  virtual nlohmann::json get(void* __ptr32 p_control_block = nullptr) = 0;
  explicit ControlBlock(const std::string& name,
                        const std::vector<std::string>& includables,
                        const std::vector<std::string>& includes)
      : control_block_name_(name), includables_(includables) {
    createIncludeMap(includes);
  }
  virtual ~ControlBlock() = default;
};

}  // namespace CBXP

#endif
