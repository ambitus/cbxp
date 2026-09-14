#include "cbxp.h"

#include <nlohmann/json.hpp>

#include "control_block_error.hpp"
#include "control_block_explorer.hpp"
#include "logger.hpp"

cbxp_result_t* cbxp_extract(const char* control_block_name,
                            const size_t control_block_name_length,
                            const char* includes, const size_t includes_length,
                            const char* filters, const size_t filters_length,
                            bool debug) {
  std::string control_block_name_string;
  std::string includes_string_string;
  std::string filters_string_string;

  if (control_block_name != nullptr) {
    control_block_name_string.assign(control_block_name,
                                     control_block_name_length);
  }
  if (includes != nullptr) {
    includes_string_string.assign(includes, includes_length);
  }
  if (filters != nullptr) {
    filters_string_string.assign(filters, filters_length);
  }

  CBXP::Logger::getInstance().setDebug(debug);

  cbxp_result_t* p_cbxp_result = new cbxp_result_t();
  CBXP::Logger::getInstance().debugAllocate(p_cbxp_result, 64,
                                            sizeof(cbxp_result_t));

  try {
    CBXP::ControlBlockExplorer explorer =
        CBXP::ControlBlockExplorer(p_cbxp_result);
    explorer.extractControlBlock(control_block_name_string,
                                 includes_string_string, filters_string_string);
  } catch (const CBXP::CBXPError& e) {
    p_cbxp_result->return_code = e.getErrorCode();
  }

  return p_cbxp_result;
}

cbxp_result_t* cbxp_format(const char* control_block_name,
                           const size_t control_block_name_length,
                           const void* data, const size_t data_length,
                           bool debug) {
  std::string control_block_name_string;

  if (control_block_name != nullptr) {
    control_block_name_string.assign(control_block_name,
                                     control_block_name_length);
  }

  CBXP::Logger::getInstance().setDebug(debug);

  cbxp_result_t* p_cbxp_result = new cbxp_result_t();
  CBXP::Logger::getInstance().debugAllocate(p_cbxp_result, 64,
                                            sizeof(cbxp_result_t));

  try {
    CBXP::ControlBlockExplorer explorer =
        CBXP::ControlBlockExplorer(p_cbxp_result);
    explorer.formatControlBlock(control_block_name_string, data, data_length);
  } catch (const CBXP::CBXPError& e) {
    p_cbxp_result->return_code = e.getErrorCode();
  }

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
