#include "include_resolver.hpp"

#include <algorithm>

#include "control_block_error.hpp"
#include "logger.hpp"

namespace CBXP {

std::unordered_map<std::string, std::unordered_map<std::string, cbxp_options_t>>
    IncludeResolver::double_asterisk_cache_ = {};

const std::unordered_map<std::string, cbxp_options_t>&
IncludeResolver::getDoubleAsteriskMap(
    const ControlBlock& cb,
    const std::unordered_map<std::string, ControlBlock>& all_cbs) {
  const std::string& cb_name = cb.getName();
  auto it                    = double_asterisk_cache_.find(cb_name);
  if (it != double_asterisk_cache_.end()) {
    Logger::getInstance().debug("Using cached '**' options map for '" +
                                cb_name + "'");
    return it->second;
  }

  Logger::getInstance().debug("Building and caching '**' options map for '" +
                              cb_name + "'");
  std::unordered_map<std::string, cbxp_options_t> built_map;
  for (const std::string& includable : cb.getIncludables()) {
    if (all_cbs.find(includable) == all_cbs.end()) {
      Logger::getInstance().debug("Skipping '" + includable +
                                  "' — not a loaded control block");
      continue;
    }
    Logger::getInstance().debug(
        "Initializing and adding '**' to the include list for the '" +
        includable + "' control block...");
    built_map[includable].include_patterns = {"**"};
  }

  auto [inserted_it, _] =
      double_asterisk_cache_.emplace(cb_name, std::move(built_map));
  return inserted_it->second;
}

std::unordered_map<std::string, cbxp_options_t> IncludeResolver::resolve(
    const std::vector<std::string>& include_patterns, const ControlBlock& cb,
    const std::unordered_map<std::string, ControlBlock>& all_cbs) {
  Logger::getInstance().debug(
      "Creating include lists for child control blocks to include with the '" +
      cb.getName() + "' control block...");

  std::unordered_map<std::string, cbxp_options_t> out;

  for (const std::string& include : include_patterns) {
    if (include == "**") {
      Logger::getInstance().debug("Processing '**' include...");
      // ** overrides everything: return the cached full-depth map directly.
      return getDoubleAsteriskMap(cb, all_cbs);
    } else if (include == "*") {
      Logger::getInstance().debug("Processing '*' include...");
      processAsterisk(out, cb, all_cbs);
    } else {
      Logger::getInstance().debug("Processing '" + include + "' include...");
      processExplicit(out, include, cb, all_cbs);
    }
  }

  Logger::getInstance().debug(
      "Include lists for child control blocks to include with the '" +
      cb.getName() + "' control block have been created");
  return out;
}

void IncludeResolver::processAsterisk(
    std::unordered_map<std::string, cbxp_options_t>& out,
    const ControlBlock& cb,
    const std::unordered_map<std::string, ControlBlock>& all_cbs) {
  for (const std::string& includable : cb.getIncludables()) {
    if (all_cbs.find(includable) == all_cbs.end()) {
      Logger::getInstance().debug("Skipping '" + includable +
                                  "' — not a loaded control block");
      continue;
    }
    if (out.find(includable) != out.end()) {
      Logger::getInstance().debug("Include list already exists for the '" +
                                  includable + "' control block");
      continue;
    }
    Logger::getInstance().debug("Initializing include list for the '" +
                                includable + "' control block...");
    out[includable].include_patterns = {};
  }
}

void IncludeResolver::processExplicit(
    std::unordered_map<std::string, cbxp_options_t>& out, std::string include,
    const ControlBlock& cb,
    const std::unordered_map<std::string, ControlBlock>& all_cbs) {
  const std::string del        = ".";
  std::string include_includes = "";
  size_t del_pos               = include.find(del);
  if (del_pos != std::string::npos) {
    include_includes = include.substr(del_pos + 1);
    include.resize(del_pos);
  }
  if (std::find(cb.getIncludables().begin(), cb.getIncludables().end(),
                include) == cb.getIncludables().end()) {
    Logger::getInstance().debug(
        "'" + include +
        "' is not a known child control block that can be included with the '" +
        cb.getName() + "' control block");
    throw IncludeError();
  }
  if (all_cbs.find(include) == all_cbs.end()) {
    Logger::getInstance().debug("'" + include +
                                "' is not a loaded control block");
    throw IncludeError();
  }
  if (out.find(include) == out.end()) {
    if (include_includes.empty()) {
      Logger::getInstance().debug("Initializing include list for the '" +
                                  include + "' control block...");
      out[include].include_patterns = {};
    } else {
      Logger::getInstance().debug("Adding '" + include_includes +
                                  "' to the include list for the '" + include +
                                  "' control block...");
      out[include].include_patterns = {include_includes};
    }
  } else {
    if (include_includes.empty()) {
      return;
    }
    if (std::find(out[include].include_patterns.begin(),
                  out[include].include_patterns.end(),
                  include_includes) != out[include].include_patterns.end()) {
      return;
    }
    Logger::getInstance().debug("Adding '" + include_includes +
                                "' to the include list for the '" + include +
                                "' control block...");
    out[include].include_patterns.push_back(include_includes);
  }
}

}  // namespace CBXP
