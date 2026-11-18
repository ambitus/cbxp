#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "cbxp_types.hpp"
#include "control_blocks/control_block.hpp"

namespace CBXP {

// Parses an include-pattern list into a per-child options map.
// Pure after construction: resolve() always returns the same result.
class IncludeResolver {
 public:
  // Returns the cached double-asterisk options map for `cb`, built on first
  // call; the cache is keyed by name and shared process-wide.
  static const std::unordered_map<std::string, cbxp_options_t>&
  getDoubleAsteriskMap(
      const ControlBlock& control_block,
      const std::unordered_map<std::string, ControlBlock>& all_control_blocks);

  // Parses `include_patterns` for `cb` and returns child-block-name ->
  // cbxp_options_t for every child to follow.
  static std::unordered_map<std::string, cbxp_options_t> resolve(
      const std::vector<std::string>& include_patterns,
      const ControlBlock& control_block,
      const std::unordered_map<std::string, ControlBlock>& all_control_blocks);

 private:
  static inline std::unordered_map<
      std::string, std::unordered_map<std::string, cbxp_options_t>>
      double_asterisk_cache_;

  static void processAsterisk(
      std::unordered_map<std::string, cbxp_options_t>& options_map,
      const ControlBlock& control_block,
      const std::unordered_map<std::string, ControlBlock>& all_control_blocks);

  static void processExplicit(
      std::unordered_map<std::string, cbxp_options_t>& options_map,
      std::string include, const ControlBlock& control_block,
      const std::unordered_map<std::string, ControlBlock>& all_control_blocks);
};

}  // namespace CBXP

