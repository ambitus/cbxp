#include "cbxp.h"

#include <pthread.h>

#include <algorithm>
#include <iostream>
#include <nlohmann/json.hpp>

#include "cbxp_result.h"
#include "control_block_explorer.hpp"

pthread_mutex_t cbxp_mutex = PTHREAD_MUTEX_INITIALIZER;

void cbxp(const char* control_block, const char* includes_string,
          const char* filters_string, bool debug, cbxp_result_t* cbxp_result) {
  nlohmann::json control_block_json;
  std::string control_block_string = control_block;

  CBXP::ControlBlockExplorer explorer =
      CBXP::ControlBlockExplorer(cbxp_result, debug);

  explorer.exploreControlBlock(control_block_string, includes_string,
                               filters_string);

  return;
}
