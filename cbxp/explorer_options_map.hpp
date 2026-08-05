#ifndef __CONTROL_BLOCK_EXPLORER_OPTIONS_MAP_H_
#define __CONTROL_BLOCK_EXPLORER_OPTIONS_MAP_H_

#include <nlohmann/json.hpp>

#include "cbxp.h"
#include "cbxp_types.hpp"
#include "control_blocks/control_block.hpp"

namespace CBXP {

typedef struct {
  std::string operation;
  std::string value;
} cbxp_filter_t;

class ExplorerOptionsMap {
 private:
  void createIncludeLists(const std::vector<std::string>& includes);
  void processDoubleAsteriskInclude();
  void processAsteriskInclude();
  void processExplicitInclude(std::string& include);
  void createFilterLists(const std::vector<std::string>& filters);
  void addCurrentFilter(const std::string& filter);
  bool compare(const nlohmann::json& json_value,
               const std::string& filter_value, const std::string& operation);
  bool matchFilter(nlohmann::json& control_block_json);
  void checkDataLength(const size_t offset) const;
  const std::vector<const void*> findControlBlockPointer(
      std::optional<ControlBlock> control_block) const;
  nlohmann::json fieldToJson(control_block_field_t field_data,
                             size_t offset = 0);
  nlohmann::json parseFields();
  std::unordered_map<std::string, cbxp_options_t> options_map_;
  std::unordered_map<std::string, std::vector<cbxp_filter_t>> current_filters_;
  bool skip_buffer_length_check_ = false;
  std::optional<ControlBlock> control_block_;
  const std::unordered_map<std::string, ControlBlock>& control_blocks_;
  const void* p_control_block_;
  const size_t buffer_length_;

 public:
  nlohmann::json getControlBlockData(const bool recursive = false);
  explicit ExplorerOptionsMap(
      const cbxp_options_t& cbxp_options, const std::string& control_block_name,
      const std::unordered_map<std::string, ControlBlock>& control_blocks,
      const void* p_control_block, const size_t buffer_length)
      : control_block_(control_blocks.at(control_block_name)),
        control_blocks_(control_blocks),
        p_control_block_(p_control_block),
        buffer_length_(buffer_length) {
    // createFilterLists depends on the construction of the "includes" portion
    // of the options_map_ structure and must be called after createIncludeLists
    ExplorerOptionsMap::createIncludeLists(cbxp_options.include_patterns);
    ExplorerOptionsMap::createFilterLists(cbxp_options.filters);
    for (auto it = options_map_.begin(); it != options_map_.end(); ++it) {
      options_map_[it->first].skip_buffer_length_check =
          skip_buffer_length_check_;
    }
  }
};
}  // namespace CBXP

#endif
