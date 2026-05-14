#ifndef __ASSB_H_
#define __ASSB_H_

#include <ihaassb.h>

#include "control_block.hpp"

namespace CBXP {

class ASSB : public ControlBlock {
 public:
  nlohmann::json get(const void* p_control_block      = nullptr,
                     const unsigned int buffer_length = 0) override;
  explicit ASSB(const cbxp_options_t& cbxp_options, bool ignore_buffer_length)
      : ControlBlock("assb", {}, cbxp_options, ignore_buffer_length) {
    control_block_length_ = sizeof(struct assb);
  }
};

}  // namespace CBXP

#endif
