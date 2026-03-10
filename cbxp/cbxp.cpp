#include "cbxp.h"

#include <algorithm>
#include <iostream>
#include <nlohmann/json.hpp>

#include "control_block_explorer.hpp"
#include "logger.hpp"

cbxp_result_t* cbxp(const char* control_block, const char* includes_string,
                    const char* filters_string, bool debug) {
  nlohmann::json control_block_json;
  std::string control_block_name = control_block;

  CBXP::Logger::getInstance().setDebug(debug);

  cbxp_result_t* p_cbxp_result = new cbxp_result_t();
  CBXP::Logger::getInstance().debugAllocate(p_cbxp_result, 64,
                                            sizeof(cbxp_result_t));

  CBXP::ControlBlockExplorer explorer =
      CBXP::ControlBlockExplorer(p_cbxp_result);

  explorer.exploreControlBlock(control_block_name, includes_string,
                               filters_string);

  return p_cbxp_result;
}

void cbxp_free(cbxp_result_t* cbxp_result, bool debug) {
  CBXP::Logger::getInstance().setDebug(debug);

  if (cbxp_result != nullptr) {
    if (cbxp_result->result_json != nullptr) {
      CBXP::Logger::getInstance().debugFree(cbxp_result->result_json);
      delete cbxp_result->result_json;
    }
    CBXP::Logger::getInstance().debugFree(cbxp_result);
    delete cbxp_result;
  }
}
