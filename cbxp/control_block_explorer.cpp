#include "control_block_explorer.hpp"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>

#include "cbxp.h"
#include "control_block_error.hpp"
#include "control_blocks/control_block.hpp"
#include "logger.hpp"
#include "traversal_engine.hpp"

namespace CBXP {

std::unordered_map<std::string, ControlBlock>
ControlBlockExplorer::loadCustomControlBlocks(
    const nlohmann::json_schema::json_validator& cbxp_schema_validator,
    const std::filesystem::path& path) {
  std::unordered_map<std::string, ControlBlock> new_maps = {};
  if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
    throw CbxpPathError();
  }
  for (const auto& file : std::filesystem::directory_iterator(path)) {
    if (file.path().extension() != ".json") {
      continue;
    }
    try {
      std::ifstream ifs(file.path());
      nlohmann::json json_data = nlohmann::json::parse(ifs);
      // The ControlBlock constructor reads fields before it validates, so the
      // mapping must be validated against the schema first.
      cbxp_schema_validator.validate(json_data);
      // Control blocks are keyed by the "name" in the mapping, not the file
      // name.
      std::string name = json_data["name"].get<std::string>();
      Logger::getInstance().debug("Adding '" + name + "' control block from '" +
                                  file.path().string() + "'.");
      new_maps.insert_or_assign(name,
                                ControlBlock(cbxp_schema_validator, json_data));
    } catch (const CBXPError&) {
      throw;
    } catch (const std::exception& e) {
      Logger::getInstance().debug("Could not load '" + file.path().string() +
                                  "': " + std::string(e.what()));
      throw CbxpJsonError();
    }
  }
  return new_maps;
}

std::vector<std::string> ControlBlockExplorer::splitCbxpPath(
    const std::string& cbxp_path) {
  std::vector<std::string> paths;
  std::stringstream stream(cbxp_path);
  std::string path;
  while (std::getline(stream, path, ':')) {
    if (!path.empty()) {
      paths.push_back(path);
    }
  }
  return paths;
}

void ControlBlockExplorer::validateControlBlockReferences(
    const std::unordered_map<std::string, ControlBlock>& control_blocks) {
  auto to_lower = [](std::string str) {
    std::transform(str.begin(), str.end(), str.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return str;
  };
  for (const auto& [name, control_block] : control_blocks) {
    const auto& parents = control_block.getPointedToBy();
    auto unknown_parent = std::find_if(
        parents.begin(), parents.end(), [&](const std::string& parent) {
          return control_blocks.find(parent) == control_blocks.end();
        });
    if (unknown_parent != parents.end()) {
      Logger::getInstance().debug("The '" + name + "' control block is " +
                                  "pointed to by unknown control block '" +
                                  *unknown_parent + "'.");
      throw CbxpJsonError();
    }
    for (const auto& [field_name, field] : control_block.getMap()) {
      if (!field.repeated) {
        continue;
      }
      const std::string count = to_lower(field.count);
      const bool count_exists = std::any_of(
          control_block.getMap().begin(), control_block.getMap().end(),
          [&](const auto& entry) { return to_lower(entry.first) == count; });
      if (!count_exists) {
        Logger::getInstance().debug(
            "The repeated field '" + field_name + "' of the '" + name +
            "' control block has an unknown count field '" + field.count +
            "'.");
        throw CbxpJsonError();
      }
    }
  }
}

std::string ControlBlockExplorer::mapToString(
    const std::unordered_map<std::string, ControlBlock>& map) {
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

const std::unordered_map<std::string, ControlBlock>&
ControlBlockExplorer::getControlBlocks() {
  nlohmann::json_schema::json_validator cbxp_schema_validator;
  cbxp_schema_validator.set_root_schema(
      nlohmann::json::parse(CBXP_SCHEMA_JSON));

  // Magic static: initialized on first call, after main() and the logger are
  // ready, so any parse/construction errors are visible in debug output.
  static std::unordered_map<std::string, ControlBlock> control_blocks = [&]() {
    std::unordered_map<std::string, ControlBlock> map;
    try {
      map = {
          { "psa", ControlBlock(cbxp_schema_validator,
           nlohmann::json::parse(PSA_JSON)) },
          { "cvt", ControlBlock(cbxp_schema_validator,
           nlohmann::json::parse(CVT_JSON)) },
          {"ecvt", ControlBlock(cbxp_schema_validator,
           nlohmann::json::parse(ECVT_JSON))},
          {"asvt", ControlBlock(cbxp_schema_validator,
           nlohmann::json::parse(ASVT_JSON))},
          {"ascb", ControlBlock(cbxp_schema_validator,
           nlohmann::json::parse(ASCB_JSON))},
          {"assb", ControlBlock(cbxp_schema_validator,
           nlohmann::json::parse(ASSB_JSON))},
          {"oucb", ControlBlock(cbxp_schema_validator,
           nlohmann::json::parse(OUCB_JSON))},
          {"ldax", ControlBlock(cbxp_schema_validator,
           nlohmann::json::parse(LDAX_JSON))},
      };
    } catch (const std::exception& e) {
      Logger::getInstance().debug("Initialization of JSON mappings failed: " +
                                  std::string(e.what()));
      return map;
    }

    // Load custom control blocks
    const char* cbxp_path_env = std::getenv("CBXPPATH");
    if (cbxp_path_env == nullptr) {
      return map;
    }
    std::string env_p(cbxp_path_env);
    if (env_p.empty()) {
      return map;
    }
    // CBXPPATH is a colon separated list of directories. Custom mappings
    // overwrite any control block of the same name that was already loaded.
    for (const auto& path : ControlBlockExplorer::splitCbxpPath(env_p)) {
      std::unordered_map<std::string, ControlBlock> custom_control_blocks =
          ControlBlockExplorer::loadCustomControlBlocks(cbxp_schema_validator,
                                                        path);
      if (!custom_control_blocks.empty()) {
        Logger::getInstance().debug(
            "Added the following custom control blocks from path '" + path +
            "': " + ControlBlockExplorer::mapToString(custom_control_blocks));
        for (auto& [name, control_block] : custom_control_blocks) {
          map.insert_or_assign(name, std::move(control_block));
        }
      }
    }
    ControlBlockExplorer::validateControlBlockReferences(map);
    return map;
  }();

  return control_blocks;
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
    const auto& control_blocks = ControlBlockExplorer::getControlBlocks();
    if (control_blocks.empty()) {
      throw CbxpJsonError();
    }
    if (control_blocks.find(control_block_name) != control_blocks.end()) {
      TraversalEngine engine(control_blocks,
                             cbxp_options_.skip_buffer_length_check);
      nlohmann::json wrapped =
          engine.run(control_block_name, cbxp_options_, p_control_block_,
                     control_block_data_length_);
      // run() wraps its result under the control block name (or name+"s" for
      // lists).  At the top level we want the unwrapped value directly.
      if (!wrapped.empty()) {
        control_block_json = wrapped.begin().value();
      }
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
