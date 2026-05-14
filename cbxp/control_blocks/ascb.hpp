#ifndef __ASCB_H_
#define __ASCB_H_

#include <ihaascb.h>

#include "control_block.hpp"

namespace CBXP {

class ASCB : public ControlBlock {
 public:
  nlohmann::json get(const void* p_control_block      = nullptr,
                     const unsigned int buffer_length = 0) override;
  explicit ASCB(const cbxp_options_t& cbxp_options, bool ignore_buffer_length)
      : ControlBlock("ascb", {"assb", "oucb"}, cbxp_options,
                     ignore_buffer_length) {
    control_block_length_ = sizeof(struct ascb);
  }
};

}  // namespace CBXP

#endif
