#include "control_block.hpp"

#include <fnmatch.h>

#include <nlohmann/json.hpp>

#include "control_block_error.hpp"
#include "logger.hpp"
#include "zos_subpools.hpp"

namespace CBXP {

FieldType ControlBlock::stringToType(const std::string& type_str) {
  if (type_str == "string") {
    return STRING;
  }
  if (type_str == "signed-integer") {
    return SIGNED_INT;
  }
  if (type_str == "unsigned-integer") {
    return UNSIGNED_INT;
  }
  if (type_str == "hex") {
    return HEX;
  }
  if (type_str == "address") {
    return ADDRESS;
  }
  throw CbxpJsonError();
}

ControlBlock::ControlBlock(
    const nlohmann::json_schema::json_validator& cbxp_schema_validator,
    const nlohmann::json& control_block_map) {
  cbxp_schema_validator.validate(control_block_map);
  control_block_length_ = 0;

  storage_attributes_.key =
      control_block_map["storageAttributes"]["key"].get<uint8_t>();
  for (auto it = control_block_map["storageAttributes"]["subpools"].begin();
       it != control_block_map["storageAttributes"]["subpools"].end(); ++it) {
    unsigned char subpool            = (*it).get<uint8_t>();
    const zos::SubpoolInfo* info     = zos::subpool_info_for(subpool);
    storage_attributes_.is_common    = zos::is_common(info->location);
    storage_attributes_.is_protected = info->fetch_protected;
  }

  for (auto it = control_block_map["pointedToBy"].begin();
       it != control_block_map["pointedToBy"].end(); ++it) {
    std::string pointed_to_by = (*it).get<std::string>();
    pointed_to_by_.push_back(pointed_to_by);
  }

  for (auto it = control_block_map["controlBlock"].begin();
       it != control_block_map["controlBlock"].end(); ++it) {
    std::string name = (*it)["name"].get<std::string>();
    size_t offset    = (*it)["offset"].get<int>();
    size_t length    = (*it)["length"].get<int>();
    if (offset + length > max_offset_) {
      max_offset_ = offset + length;
    }
    std::string pointsTo = "";
    if ((*it).contains("pointsTo")) {
      // Also build inclusion MAP
      pointsTo = (*it)["pointsTo"].get<std::string>();
      includables_.push_back(pointsTo);
    }
    std::string type_str = (*it)["type"].get<std::string>();
    FieldType type       = ControlBlock::stringToType(type_str);
    bool repeated        = false;
    std::string count    = "";
    if ((*it).contains("repeated")) {
      repeated = (*it)["repeated"].get<bool>();
    }
    if (repeated and (*it).contains("count")) {
      count = (*it)["count"].get<std::string>();
    }
    control_block_field_t field = {name,     offset,   length, type,
                                   pointsTo, repeated, count};
    control_block_map_[name]    = field;
  }
}
}  // namespace CBXP

