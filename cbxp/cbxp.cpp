#include "cbxp.h"

#include <algorithm>
#include <iostream>
#include <nlohmann/json.hpp>

#include "cbxp_result.h"
#include "control_block_explorer.hpp"

cbxp_result_t* cbxp(const char* control_block, const char* includes_string,
                    const char* filters_string, bool debug) {
  nlohmann::json control_block_json;
  std::string control_block_string = control_block;

  cbxp_result_t* cbxp_result       = new cbxp_result_t({nullptr, 0, -1});

  CBXP::ControlBlockExplorer explorer =
      CBXP::ControlBlockExplorer(cbxp_result, debug);

  explorer.exploreControlBlock(control_block_string, includes_string,
                               filters_string);

  return cbxp_result;
}

void cbxp_free(cbxp_result_t* cbxp_result) {
  delete cbxp_result->result_json;
  delete cbxp_result;
}
