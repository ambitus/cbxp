#include "control_block.hpp"

#include <nlohmann/json.hpp>

#include "control_block_error.hpp"
#include "logger.hpp"

namespace CBXP {
void ControlBlock::createIncludeMap(const std::vector<std::string>& includes) {
  Logger::getInstance().debug("Creating include map for the '" +
                              control_block_name_ + "' control block...");
  for (std::string include : includes) {
    if (include == "**") {
      ControlBlock::processDoubleAsteriskInclude();
      return;
    } else if (include == "*") {
      ControlBlock::processAsteriskInclude();
    } else {
      ControlBlock::processExplicitInclude(include);
    }
  }
  Logger::getInstance().debug("Done");
}

void ControlBlock::processDoubleAsteriskInclude() {
  // Any existing entries in the hash map are redundant, so clear them
  include_map_.clear();
  for (const std::string& includable : includables_) {
    // Build a map of all includables_ but with "**" at the next level
    include_map_[includable] = {"**"};
  }
}

void ControlBlock::processAsteriskInclude() {
  if (include_map_.empty()) {
    for (const std::string& includable : includables_) {
      // Build a map of all includables_
      include_map_[includable] = {};
    }
  }
  for (const std::string& includable : includables_) {
    if (include_map_.find(includable) != include_map_.end()) {
      continue;
    }
    // Add all includables_ not already present to the map
    include_map_[includable] = {};
  }
}

void ControlBlock::processExplicitInclude(std::string& include) {
  // Default case; have to validate against an includable
  const std::string del        = ".";
  std::string include_includes = "";
  size_t del_pos               = include.find(del);
  if (del_pos != std::string::npos) {
    // If there's a "." then separate include into the include and its
    // includes
    include_includes = include.substr(del_pos + 1);
    include.resize(del_pos);
  }
  if (std::find(includables_.begin(), includables_.end(), include) ==
      includables_.end()) {
    Logger::getInstance().debug("'" + include +
                                "' is not a known includable for the '" +
                                control_block_name_ + "' control block");
    throw IncludeError();
  }
  if (include_map_.find(include) == include_map_.end()) {
    // If we don't already have this include in our map, add it with its
    // includes
    if (include_includes == "") {
      include_map_[include] = {};
    } else {
      include_map_[include] = {include_includes};
    }
  } else {
    // If we DO already have this in our map, then we should add its
    // includes if they are useful or new
    if (std::find(include_map_[include].begin(), include_map_[include].end(),
                  include_includes) != include_map_[include].end()) {
      return;
    }
    if (include_includes == "") {
      return;
    }
    include_map_[include].push_back(include_includes);
  }
}
}  // namespace CBXP

