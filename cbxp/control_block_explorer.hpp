#ifndef __CONTROL_BLOCK_EXPLORER_H_
#define __CONTROL_BLOCK_EXPLORER_H_

#include <nlohmann/json.hpp>

#include "cbxp.h"
#include "cbxp_types.hpp"
#include "control_blocks/control_block.hpp"
#include "explorer_options_map.hpp"

namespace CBXP {

class ControlBlockExplorer {
 private:
  cbxp_options_t cbxp_options_ = {{}, {}, false};
  cbxp_result_t* p_result_;
  const void* p_control_block_         = nullptr;
  size_t control_block_data_length_    = 0;
  std::string control_block_operation_ = "";

  static std::unordered_map<std::string, ControlBlock> control_blocks_;
  static std::string mapToString(
      const std::unordered_map<std::string, ControlBlock>& map);
  static std::unordered_map<std::string, ControlBlock> loadCustomControlBlocks(
      std::filesystem::path path);
  static std::unordered_map<std::string, ControlBlock> buildControlBlockMap();
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
