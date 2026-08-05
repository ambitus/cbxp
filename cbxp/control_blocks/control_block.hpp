#ifndef __CONTROL_BLOCK_H_
#define __CONTROL_BLOCK_H_

#include <nlohmann/json-schema.hpp>
#include <nlohmann/json.hpp>

#include "control_block_field_formatter.hpp"

namespace CBXP {

enum FieldType { STRING, SIGNED_INT, UNSIGNED_INT, HEX, ADDRESS };

typedef struct {
  std::string name;
  size_t offset;
  size_t length;
  FieldType type;
  std::string pointsTo;
  bool repeated;
  std::string count;
} control_block_field_t;

typedef struct {
  bool is_common;
  bool is_protected;
  unsigned char key;
} storage_attributes_t;

class ControlBlock {
 private:
  static const nlohmann::json_schema::json_validator cbxp_schema_validator_;
  std::unordered_map<std::string, control_block_field_t> control_block_map_ =
      {};
  storage_attributes_t storage_attributes_ = {
      true, false, 8};  // actually establish these values as this is finalized
  std::string control_block_name_;
  std::vector<std::string> includables_   = {};
  std::vector<std::string> pointed_to_by_ = {};
  size_t control_block_length_            = 0;
  size_t max_offset_                      = 0;
  void* fixed_address_                    = nullptr;
  static FieldType stringToType(const std::string& type_str);

 public:
  ControlBlockFieldFormatter formatter_;
  const std::unordered_map<std::string, control_block_field_t>& getMap() const {
    return control_block_map_;
  }
  const storage_attributes_t& getStorageAttributes() const {
    return storage_attributes_;
  }
  const std::string& getName() const { return control_block_name_; }
  const std::vector<std::string>& getIncludables() const {
    return includables_;
  }
  const std::vector<std::string>& getPointedToBy() const {
    return pointed_to_by_;
  }
  size_t getMaxOffset() const { return max_offset_; }
  const void* getFixedAddress() const { return fixed_address_; }
  explicit ControlBlock(
      const nlohmann::json_schema::json_validator& cbxp_schema_validator,
      const nlconst nlohmann::json& control_block_map);
};

}  // namespace CBXP

#endif
