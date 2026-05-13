#include "cbxp.h"

#include <nlohmann/json.hpp>

#include "control_block_explorer.hpp"
#include "logger.hpp"

cbxp_result_t* cbxp_extract(const char* control_block,
                            const int control_block_length,
                            const char* includes, const int includes_length,
                            const char* filters, const int filters_length,
                            bool debug) {
  nlohmann::json control_block_json;

  std::string control_block_string;
  std::string includes_string_string;
  std::string filters_string_string;

  if (control_block != nullptr) {
    control_block_string.assign(control_block,
                                control_block + control_block_length);
  }
  if (includes != nullptr) {
    includes_string_string.assign(includes, includes_length);
  }
  if (filters != nullptr) {
    filters_string_string.assign(filters, filters + filters_length);
  }

  CBXP::Logger::getInstance().setDebug(debug);

  cbxp_result_t* p_cbxp_result = new cbxp_result_t();
  CBXP::Logger::getInstance().debugAllocate(p_cbxp_result, 64,
                                            sizeof(cbxp_result_t));

  CBXP::ControlBlockExplorer explorer =
      CBXP::ControlBlockExplorer(p_cbxp_result);

  explorer.extractControlBlock(control_block_string, includes_string_string,
                               filters_string_string);

  return p_cbxp_result;
}

cbxp_result_t* cbxp_format(const char* control_block,
                           const int control_block_length,
                           const void* bytes_buffer, const int buffer_length,
                           bool debug) {
  nlohmann::json control_block_json;

  std::string control_block_string;

  if (control_block != nullptr) {
    control_block_string.assign(control_block,
                                control_block + control_block_length);
  }

  CBXP::Logger::getInstance().setDebug(debug);

  cbxp_result_t* p_cbxp_result = new cbxp_result_t();
  CBXP::Logger::getInstance().debugAllocate(p_cbxp_result, 64,
                                            sizeof(cbxp_result_t));

  CBXP::ControlBlockExplorer explorer =
      CBXP::ControlBlockExplorer(p_cbxp_result);

  explorer.formatControlBlock(control_block_string, bytes_buffer,
                              buffer_length);

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
