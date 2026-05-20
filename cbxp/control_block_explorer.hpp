#ifndef __CONTROL_BLOCK_EXPLORER_H_
#define __CONTROL_BLOCK_EXPLORER_H_

#include <nlohmann/json.hpp>

#include "cbxp.h"
#include "control_blocks/control_block.hpp"

namespace CBXP {

class ControlBlockExplorer {
 private:
  cbxp_result_t* p_result_;
  cbxp_options_t cbxp_options_         = {{}, {}, false};
  const void* p_control_block_         = nullptr;
  size_t control_block_data_length_    = 0;
  std::string control_block_operation_ = "";
  static std::vector<std::string> createOptionsList(
      const std::string& comma_separated_string);

 public:
  explicit ControlBlockExplorer(cbxp_result_t* p_result)
      : p_result_(p_result) {};
  void extractControlBlock(const std::string& control_block_name,
                           const std::string& includes_string,
                           const std::string& filters_string);
  void formatControlBlock(const std::string& control_block_name,
                          const void* p_data, const size_t data_length);
  void processControlBlock(const std::string& control_block_name);
};
}  // namespace CBXP

#endif
