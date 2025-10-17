#include "cbxp.h"

#include <algorithm>
#include <iostream>
#include <nlohmann/json.hpp>

#include "cbxp_result.h"
#include "control_block_explorer.hpp"

cbxp_result_t* cbxp(const char* control_block_name, const char* includes_string,
                    bool debug) {
  nlohmann::json control_block_json;
  std::string control_block        = control_block_name;

  static cbxp_result_t cbxp_result = {nullptr, 0, -1};

  CBXP::ControlBlockExplorer explorer =
      CBXP::ControlBlockExplorer(&cbxp_result, debug);

  explorer.exploreControlBlock(control_block, includes_string);

  return &cbxp_result;
}
