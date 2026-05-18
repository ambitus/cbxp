#ifndef __ASCB_H_
#define __ASCB_H_

#include <ihaascb.h>

#include "control_block.hpp"

namespace CBXP {

class ASCB : public ControlBlock {
 public:
  nlohmann::json get(const void* p_control_block = nullptr,
                     const size_t buffer_length  = 0) override;
  explicit ASCB(const cbxp_options_t& cbxp_options)
      : ControlBlock("ascb", {"assb", "oucb"}, cbxp_options) {
    control_block_length_ = sizeof(struct ascb);
  }
};

}  // namespace CBXP

#endif
