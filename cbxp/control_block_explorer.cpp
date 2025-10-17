#include "control_block_explorer.hpp"

#include <algorithm>
#include <iostream>
#include <nlohmann/json.hpp>

#include "cbxp.h"
#include "control_block_error.hpp"
#include "control_blocks/ascb.hpp"
#include "control_blocks/asvt.hpp"
#include "control_blocks/control_block.hpp"
#include "control_blocks/cvt.hpp"
#include "control_blocks/ecvt.hpp"
#include "control_blocks/psa.hpp"
#include "logger.hpp"

namespace CBXP {

std::vector<std::string> ControlBlockExplorer::createIncludeList(
    const std::string& includes_string) {
  if (includes_string == "") {
    return {};
  }

  std::vector<std::string> includes = {};

  Logger::getInstance().debug(
      "Creating include list from the provided include list string: " +
      includes_string);

  const std::string del = ",";
  std::string entry;
  size_t index = 0;

  auto pos     = includes_string.find(del);

  while (pos != std::string::npos) {
    entry = includes_string.substr(index, pos);
    includes.push_back(entry);
    index += pos + 1;
    pos = includes_string.substr(index, std::string::npos).find(del);
  }
  entry = includes_string.substr(index, pos);
  includes.push_back(entry);
  Logger::getInstance().debug("Done.");

  return includes;
}

ControlBlockExplorer::ControlBlockExplorer(cbxp_result_t* p_result,
                                           bool debug) {
  Logger::getInstance().setDebug(debug);

  if (p_result->result_json != nullptr) {
    Logger::getInstance().debugFree(p_result->result_json);
    delete[] p_result->result_json;
  }

  p_result->result_json_length = 0;
  p_result->result_json        = nullptr;
  p_result->return_code        = 0;

  p_result_                    = p_result;
}

void ControlBlockExplorer::exploreControlBlock(
    const std::string& control_block_name, const std::string& includes_string) {
  std::vector<std::string> includes =
      ControlBlockExplorer::createIncludeList(includes_string);

  Logger::getInstance().debug("Extracting '" + control_block_name +
                              "' control block data...");

  nlohmann::json control_block_json;
  try {
    if (control_block_name == "psa") {
      control_block_json = PSA(includes).get();
    } else if (control_block_name == "cvt") {
      control_block_json = CVT(includes).get();
    } else if (control_block_name == "ecvt") {
      control_block_json = ECVT(includes).get();
    } else if (control_block_name == "ascb") {
      control_block_json = ASCB(includes).get();
    } else if (control_block_name == "asvt") {
      control_block_json = ASVT(includes).get();
    } else {
      throw ControlBlockError();
    }
  } catch (const CBXPError& e) {
    p_result_->return_code = e.getErrorCode();
    return;
  }

  std::string control_block_json_string = control_block_json.dump();

  Logger::getInstance().debug("Done.");

  Logger::getInstance().debug("Control Block JSON: " +
                              control_block_json_string);

  p_result_->result_json_length = control_block_json_string.length();
  p_result_->result_json        = new char[p_result_->result_json_length];
  p_result_->result_json[p_result_->result_json_length] = 0;

  Logger::getInstance().debugAllocate(p_result_->result_json, 64,
                                      p_result_->result_json_length);

  std::strncpy(p_result_->result_json, control_block_json_string.c_str(),
               p_result_->result_json_length);

  return;
}

}  // namespace CBXP
