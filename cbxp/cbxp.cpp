#include "cbxp.h"

#include <pthread.h>

#include <algorithm>
#include <iostream>
#include <nlohmann/json.hpp>

#include "cbxp_result.h"
#include "control_block_explorer.hpp"

pthread_mutex_t cbxp_mutex = PTHREAD_MUTEX_INITIALIZER;

cbxp_result_t* cbxp(const char* control_block, const char* includes_string,
                    const char* filters_string, bool debug) {
  // Since cbxp manages cbxp_result_t as a static structure,
  // we need to use a mutex to make this thread safe.
  // Technically we shouldn't need this because the Python GIL,
  // but we will set this up anyways to be safe.
  pthread_mutex_lock(&cbxp_mutex);

  nlohmann::json control_block_json;
  std::string control_block_string = control_block;

  static cbxp_result_t cbxp_result = {nullptr, 0, -1};

  CBXP::ControlBlockExplorer explorer =
      CBXP::ControlBlockExplorer(&cbxp_result, debug);

  explorer.exploreControlBlock(control_block_string, includes_string,
                               filters_string);

  pthread_mutex_unlock(&cbxp_mutex);

  return &cbxp_result;
}
