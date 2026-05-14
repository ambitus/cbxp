#ifndef __CVT_H_
#define __CVT_H_

#include <cvt.h>

#include "control_block.hpp"

namespace CBXP {

class CVT : public ControlBlock {
 public:
  nlohmann::json get(const void* p_control_block      = nullptr,
                     const unsigned int buffer_length = 0) override;
  explicit CVT(const cbxp_options_t& cbxp_options, bool ignore_buffer_length)
      : ControlBlock("cvt", {"ecvt", "asvt"}, cbxp_options,
                     ignore_buffer_length) {
    control_block_length_ = sizeof(struct cvtmap);
  }
};

}  // namespace CBXP

#endif
