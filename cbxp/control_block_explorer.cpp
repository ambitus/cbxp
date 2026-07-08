#include "control_block_explorer.hpp"

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <nlohmann/json.hpp>

#include "cbxp.h"
#include "control_block_error.hpp"
#include "control_blocks/control_block.hpp"
#include "logger.hpp"

namespace CBXP {

std::unordered_map<std::string, ControlBlockMap>
ControlBlockExplorer::loadCustomControlBlocks(std::filesystem::path path) {
  std::unordered_map<std::string, ControlBlockMap> new_maps = {};
  try {
    if (!std::filesystem::exists(path) ||
        !std::filesystem::is_directory(path)) {
      throw CbxpPathError();
    }
    for (const auto& file : std::filesystem::directory_iterator(dir_path)) {
      if (file.extension() != ".json") {
        continue;
      }
      std::ifstream ifs(file);
      nlohmann::json json_data = nlohmann::json::parse(ifs);
      Logger::getInstance().debug("Adding '" + file_name +
                                  "' control block from '" + file + "'.");
      ControlBlockMap new_map = ControlBlockMap(json_data);
      std::string file_name   = file.stem().string();
      new_maps[file_name]     = new_map;
    }
  } catch (const std::filesystem::filesystem_error& e) {
    throw CbxpPathError();
  } catch (const std::exception& e) {
    throw CbxpJsonError();
  }
  return new_maps;
}

std::string ControlBlockExplorer::mapToString(
    const std::unordered_map<std::string, ControlBlockMap>& map) {
  std::string map_as_string = "";
  if (map.empty()) {
    return map_as_string;
  }
  map_as_string    = "[";

  bool first_entry = true;
  for (const auto& [key, value] : map) {
    if (!first_entry) {
      map_as_string += ", ";
    } else {
      first_entry = false;
    }
    map_as_string += key;
  }

  map_as_string += "]";
  return map_as_string;
}

static std::unordered_map<std::string, ControlBlockMap>
ControlBlockExplorer::buildControlBlockMap() {
  // Load known control blocks
  std::unordered_map<std::string, ControlBlockMap> control_blocks = {
      { "psa",  ControlBlockMap(PSA_JSON)},
      { "cvt",  ControlBlockMap(CVT_JSON)},
      {"ecvt", ControlBlockMap(ECVT_JSON)},
      {"asvt", ControlBlockMap(ASVT_JSON)},
      {"ascb", ControlBlockMap(ASCB_JSON)},
      {"assb", ControlBlockMap(ASSB_JSON)},
      {"oucb", ControlBlockMap(OUCB_JSON)},
      {"ldax", ControlBlockMap(LDAX_JSON)},
  };

  // Load custom control blocks
  std::string env_p(std::getenv("CBXPPATH"));
  if (env_p.empty()) {
    return control_blocks;
  }
  // Logic for loading custom control block mappings, overwriting any that were
  // already loaded with custom mappings as well.
  std::filesystem::path cbxp_path(env_p);
  for (const auto& path : cbxp_path) {
    std::unordered_map<std::string, ControlBlockMap> custom_control_blocks =
        ControlBlockExplorer::loadCustomControlBlocks(path);
    if (!custom_control_blocks.empty()) {
      Logger::getInstance().debug(
          "Added the following custom control blocks from path '" + path +
          "': " + ControlBlockExplorer::mapToString(custom_control_blocks));
      custom_control_blocks.insert(control_blocks.begin(),
                                   control_blocks.end());
    }
  }
}

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
    const std::string& control_block_name, const void* p_data,
    const size_t data_length) {
  if (p_data == nullptr) {
    // C/C++ callers who invoke this directly could pass null pointer(s) in
    // which should halt processing
    p_result_->return_code = Error::NullDataPtr;
    return;
  }
  p_control_block_           = p_data;
  control_block_data_length_ = data_length;

  control_block_operation_   = "Format";

  ControlBlockExplorer::processControlBlock(control_block_name);

  return;
}

void ControlBlockExplorer::processControlBlock(
    const std::string& control_block_name) {
  Logger::getInstance().debug(control_block_operation_ + "ing '" +
                              control_block_name + "' control block data...");

  nlohmann::json control_block_json = {};

  try {
    if (control_blocks_.contains(control_block_name)) {
      explorer_options_ =
          ExplorerOptionsMap(cbxp_options_, control_block_name, control_blocks_,
                             p_control_block_, control_block_data_length_);
      control_block_json = explorer_options_.getControlBlockData();
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
