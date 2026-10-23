#ifndef __CBXP_INCLUDE_RESOLVER_H_
#define __CBXP_INCLUDE_RESOLVER_H_

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
  // Returns the double-asterisk options map for `cb`, building and caching
  // it on first call.  The cache is keyed by control-block name and is
  // shared across all IncludeResolver instances for the process lifetime.
  static const std::unordered_map<std::string, cbxp_options_t>&
  getDoubleAsteriskMap(
      const ControlBlock& cb,
      const std::unordered_map<std::string, ControlBlock>& all_cbs);

  // Parse `include_patterns` in the context of `cb` and return a map of
  // child-control-block-name → cbxp_options_t for every child that should
  // be followed.
  static std::unordered_map<std::string, cbxp_options_t> resolve(
      const std::vector<std::string>& include_patterns, const ControlBlock& cb,
      const std::unordered_map<std::string, ControlBlock>& all_cbs);

 private:
  static std::unordered_map<std::string,
                            std::unordered_map<std::string, cbxp_options_t>>
      double_asterisk_cache_;

  static void processAsterisk(
      std::unordered_map<std::string, cbxp_options_t>& out,
      const ControlBlock& cb,
      const std::unordered_map<std::string, ControlBlock>& all_cbs);

  static void processExplicit(
      std::unordered_map<std::string, cbxp_options_t>& out, std::string include,
      const ControlBlock& cb,
      const std::unordered_map<std::string, ControlBlock>& all_cbs);
};

}  // namespace CBXP

#endif
