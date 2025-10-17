#ifndef __CONTROL_BLOCK_EXPLORER_H_
#define __CONTROL_BLOCK_EXPLORER_H_

#include <nlohmann/json.hpp>

#include "cbxp.h"

namespace CBXP {
class ControlBlockExplorer {
 private:
  cbxp_result_t* p_result_;
  static std::vector<std::string> createIncludeList(
      const std::string& includes_string);

 public:
  ControlBlockExplorer(cbxp_result_t* p_result, bool debug);
  void exploreControlBlock(const std::string& control_block_name,
                           const std::string& includes_string);
};
}  // namespace CBXP

#endif
