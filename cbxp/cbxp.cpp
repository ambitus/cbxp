#include "cbxp.h"

#include <algorithm>
#include <iostream>
#include <nlohmann/json.hpp>

#include "cbxp_result.h"
#include "control_block_explorer.hpp"
#include "logger.hpp"

cbxp_result_t* cbxp(const char* control_block, const char* includes_string,
                    const char* filters_string, bool debug) {
  nlohmann::json control_block_json;
  std::string control_block_string = control_block;

  CBXP::Logger::getInstance().setDebug(debug);

  CBXP::Logger::getInstance().debug("Allocating cbxp result object...");
  cbxp_result_t* cbxp_result = new cbxp_result_t({nullptr, 0, -1});
  CBXP::Logger::getInstance().debug("Cbxp result object allocated.");

  CBXP::ControlBlockExplorer explorer = CBXP::ControlBlockExplorer(cbxp_result);

  explorer.exploreControlBlock(control_block_string, includes_string,
                               filters_string);

  return cbxp_result;
}

void cbxp_free(cbxp_result_t* cbxp_result, bool debug) {
  CBXP::Logger::getInstance().setDebug(debug);

  CBXP::Logger::getInstance().debug("Deleting cbxp result json...");
  delete cbxp_result->result_json;
  CBXP::Logger::getInstance().debug("Cbxp result json deleted.");
  CBXP::Logger::getInstance().debug("Deleting cbxp result object...");
  delete cbxp_result;
  CBXP::Logger::getInstance().debug("Cbxp result object deleted.");
}
