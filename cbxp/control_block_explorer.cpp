#include "control_block_explorer.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <nlohmann/json.hpp>

#include "cbxp.h"
#include "control_block_error.hpp"
#include "control_blocks/ascb.hpp"
#include "control_blocks/assb.hpp"
#include "control_blocks/asvt.hpp"
#include "control_blocks/control_block.hpp"
#include "control_blocks/cvt.hpp"
#include "control_blocks/ecvt.hpp"
#include "control_blocks/oucb.hpp"
#include "control_blocks/psa.hpp"
#include "logger.hpp"

namespace CBXP {

std::vector<std::string> ControlBlockExplorer::createOptionsList(
    const std::string& comma_separated_string) {
  if (comma_separated_string == "") {
    return {};
  }

  std::vector<std::string> options_list = {};

  Logger::getInstance().debug(
      "Creating options list from the provided comma-separated list string: " +
      comma_separated_string);

  const std::string del = ",";
  std::string entry;
  size_t index = 0;

  auto pos     = comma_separated_string.find(del);

  while (pos != std::string::npos) {
    entry = comma_separated_string.substr(index, pos);
    options_list.push_back(entry);
    index += pos + 1;
    pos = comma_separated_string.substr(index, std::string::npos).find(del);
  }
  entry = comma_separated_string.substr(index, pos);
  options_list.push_back(entry);
  Logger::getInstance().debug("Options list created");

  return options_list;
}

void ControlBlockExplorer::extractControlBlock(
    const std::string& control_block_name, const std::string& includes_string,
    const std::string& filters_string) {
  cbxp_options_ = {ControlBlockExplorer::createOptionsList(includes_string),
                   ControlBlockExplorer::createOptionsList(filters_string),
                   true};

  control_block_operation_ = "Extract";

  ControlBlockExplorer::processControlBlock(control_block_name);

  return;
}

void ControlBlockExplorer::formatControlBlock(
    const std::string& control_block_name, const void* data,
    const size_t data_length) {
  p_control_block_ = data;
  if (p_control_block_ == nullptr) {
    control_block_data_length_ = 0;
  } else {
    control_block_data_length_ = data_length;
  }

  control_block_operation_ = "Format";

  ControlBlockExplorer::processControlBlock(control_block_name);

  return;
}

void ControlBlockExplorer::processControlBlock(
    const std::string& control_block_name) {
  Logger::getInstance().debug(control_block_operation_ + "ing '" +
                              control_block_name + "' control block data...");

  nlohmann::json control_block_json;

  try {
    if (control_block_name == "psa") {
      control_block_json =
          PSA(cbxp_options_).get(p_control_block_, control_block_data_length_);
    } else if (control_block_name == "cvt") {
      control_block_json =
          CVT(cbxp_options_).get(p_control_block_, control_block_data_length_);
    } else if (control_block_name == "ecvt") {
      control_block_json =
          ECVT(cbxp_options_).get(p_control_block_, control_block_data_length_);
    } else if (control_block_name == "ascb") {
      control_block_json =
          ASCB(cbxp_options_).get(p_control_block_, control_block_data_length_);
    } else if (control_block_name == "asvt") {
      control_block_json =
          ASVT(cbxp_options_).get(p_control_block_, control_block_data_length_);
    } else if (control_block_name == "assb") {
      control_block_json =
          ASSB(cbxp_options_).get(p_control_block_, control_block_data_length_);
    } else if (control_block_name == "oucb") {
      control_block_json =
          OUCB(cbxp_options_).get(p_control_block_, control_block_data_length_);
    } else {
      throw ControlBlockError();
    }
  } catch (const CBXPError& e) {
    p_result_->return_code = e.getErrorCode();
    return;
  }

  std::string control_block_json_string = control_block_json.dump(
      -1, ' ', false, nlohmann::json::error_handler_t::replace);

  Logger::getInstance().debug(control_block_operation_ + "ed '" +
                              control_block_name + "' control block data");

  Logger::getInstance().debug("Control Block JSON: " +
                              control_block_json_string);

  p_result_->result_json_length = control_block_json_string.length();
  p_result_->result_json        = new char[p_result_->result_json_length + 1];
  Logger::getInstance().debugAllocate(p_result_->result_json, 64,
                                      p_result_->result_json_length + 1);

  std::memcpy(p_result_->result_json, control_block_json_string.c_str(),
              p_result_->result_json_length);

  p_result_->result_json[p_result_->result_json_length] = 0;

  return;
}

}  // namespace CBXP
