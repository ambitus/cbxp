#ifndef __CONTROL_BLOCK_EXPLORER_H_
#define __CONTROL_BLOCK_EXPLORER_H_

#include <nlohmann/json.hpp>

#include "cbxp.h"
#include "control_blocks/control_block.hpp"

namespace CBXP {
class ControlBlockExplorer {
 private:
  cbxp_result_t* p_result_;
  cbxp_options_t cbxp_options                = {{}, {}};
  void* control_block_pointer                = nullptr;
  const uint64_t control_block_buffer_length = -1;
  std::string control_block_operation        = "";
  static std::vector<std::string> createOptionsList(
      const std::string& comma_separated_string);

 public:
  explicit ControlBlockExplorer(cbxp_result_t* p_result)
      : p_result_(p_result) {};
  void exploreControlBlock(const std::string& control_block_name,
                           const std::string& includes_string,
                           const std::string& filters_string);
  void formatControlBlock(const std::string& control_block_name,
                          void* bytes_buffer, const uint64_t buffer_length);
  void processControlBlock(const std::string& control_block_name);
};
}  // namespace CBXP

#endif
