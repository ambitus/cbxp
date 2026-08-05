#ifndef __CBXP_TYPES_H_
#define __CBXP_TYPES_H_

#include <string>
#include <vector>

namespace CBXP {

typedef struct {
  std::vector<std::string> include_patterns;
  std::vector<std::string> filters;
  bool skip_buffer_length_check;
} cbxp_options_t;

}  // namespace CBXP

#endif
