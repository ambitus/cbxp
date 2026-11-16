#pragma once

#include <string>
#include <vector>

namespace CBXP {

typedef struct {
  std::vector<std::string> include_patterns;
  std::vector<std::string> filters;
} cbxp_options_t;

}  // namespace CBXP

