#ifndef __ASCB_H_
#define __ASCB_H_

#include <ihaascb.h>

#include "control_block.hpp"

namespace CBXP {

class ASCB : public ControlBlock {
 public:
  nlohmann::json get(void* p_control_block        = nullptr,
                     const uint64_t buffer_length = -1) override;
  explicit ASCB(const cbxp_options_t& cbxp_options)
      : ControlBlock("ascb", {"assb", "oucb"}, cbxp_options) {
    control_block_length = sizeof(struct ascb);
  }
};

}  // namespace CBXP

#endif
