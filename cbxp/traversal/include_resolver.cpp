#include "traversal/include_resolver.hpp"

#include <algorithm>

#include "control_block_error.hpp"
#include "util/logger.hpp"

namespace CBXP {

const std::unordered_map<std::string, cbxp_options_t>&
IncludeResolver::getDoubleAsteriskMap(
    const ControlBlock& cb,
    const std::unordered_map<std::string, ControlBlock>& all_cbs) {
  const std::string& control_block_name = control_block.getName();
  auto it = double_asterisk_cache_.find(control_block_name);
  if (it != double_asterisk_cache_.end()) {
    Logger::getInstance().debug("Using cached '**' options map for '" +
                                control_block_name + "'");
    return it->second;
  }

  Logger::getInstance().debug("Building and caching '**' options map for '" +
                              control_block_name + "'");
  std::unordered_map<std::string, cbxp_options_t> built_map;
  for (const std::string& includable : control_block.getIncludables()) {
    if (all_cbs.find(includable) == all_cbs.end()) {
      Logger::getInstance().debug("Skipping '" + includable +
                                  "' - not a loaded control block");
      continue;
    }
    Logger::getInstance().debug(
        "Initializing and adding '**' to the include list for the '" +
        includable + "' control block...");
    built_map[includable].include_patterns = {"**"};
  }

  auto [inserted_it, _] =
      double_asterisk_cache_.emplace(control_block_name, std::move(built_map));
  return inserted_it->second;
}

std::unordered_map<std::string, cbxp_options_t> IncludeResolver::resolve(
    const std::vector<std::string>& include_patterns, const ControlBlock& cb,
    const std::unordered_map<std::string, ControlBlock>& all_cbs) {
  Logger::getInstance().debug(
      "Creating include lists for child control blocks to include with the '" +
      control_block.getName() + "' control block...");

  std::unordered_map<std::string, cbxp_options_t> options_map;

  for (const std::string& include : include_patterns) {
    if (include == "**") {
      Logger::getInstance().debug("Processing '**' include...");
      // ** overrides everything: return the cached full-depth map directly.
      return IncludeResolver::getDoubleAsteriskMap(cb, all_cbs);
    } else if (include == "*") {
      Logger::getInstance().debug("Processing '*' include...");
      IncludeResolver::processAsterisk(options_map, cb, all_cbs);
    } else {
      Logger::getInstance().debug("Processing '" + include + "' include...");
      IncludeResolver::processExplicit(options_map, include, cb, all_cbs);
    }
  }

  Logger::getInstance().debug(
      "Include lists for child control blocks to include with the '" +
      control_block.getName() + "' control block have been created");
  return options_map;
}

void IncludeResolver::processAsterisk(
    std::unordered_map<std::string, cbxp_options_t>& options_map,
    const ControlBlock& cb,
    const std::unordered_map<std::string, ControlBlock>& all_cbs) {
  for (const std::string& includable : control_block.getIncludables()) {
    if (all_cbs.find(includable) == all_cbs.end()) {
      Logger::getInstance().debug("Skipping '" + includable +
                                  "' - not a loaded control block");
      continue;
    }
    if (options_map.find(includable) != options_map.end()) {
      Logger::getInstance().debug("Include list already exists for the '" +
                                  includable + "' control block");
      continue;
    }
    Logger::getInstance().debug("Initializing include list for the '" +
                                includable + "' control block...");
    options_map[includable].include_patterns = {};
  }
}

void IncludeResolver::processExplicit(
    std::unordered_map<std::string, cbxp_options_t>& options_map,
    std::string include, const ControlBlock& cb,
    const std::unordered_map<std::string, ControlBlock>& all_cbs) {
  const std::string del        = ".";
  std::string include_includes = "";
  size_t del_pos               = include.find(del);
  if (del_pos != std::string::npos) {
    include_includes = include.substr(del_pos + 1);
    include.resize(del_pos);
  }
  if (std::find(control_block.getIncludables().begin(),
                control_block.getIncludables().end(),
                include) == control_block.getIncludables().end()) {
    Logger::getInstance().debug(
        "'" + include +
        "' is not a known child control block that can be included with the '" +
        control_block.getName() + "' control block");
    throw IncludeError();
  }
  if (all_cbs.find(include) == all_cbs.end()) {
    Logger::getInstance().debug("'" + include +
                                "' is not a loaded control block");
    throw IncludeError();
  }
  if (options_map.find(include) == options_map.end()) {
    if (include_includes.empty()) {
      Logger::getInstance().debug("Initializing include list for the '" +
                                  include + "' control block...");
      options_map[include].include_patterns = {};
    } else {
      Logger::getInstance().debug("Adding '" + include_includes +
                                  "' to the include list for the '" + include +
                                  "' control block...");
      options_map[include].include_patterns = {include_includes};
    }
  } else {
    if (include_includes.empty()) {
      return;
    }
    if (std::find(options_map[include].include_patterns.begin(),
                  options_map[include].include_patterns.end(),
                  include_includes) !=
        options_map[include].include_patterns.end()) {
      return;
    }
    Logger::getInstance().debug("Adding '" + include_includes +
                                "' to the include list for the '" + include +
                                "' control block...");
    options_map[include].include_patterns.push_back(include_includes);
  }
}

}  // namespace CBXP
