#ifndef __ASSB_H_
#define __ASSB_H_

#include <ihaassb.h>

#include "control_block.hpp"

namespace CBXP {

class ASSB : public ControlBlock {
 public:
  nlohmann::json get(const void* p_control_block = nullptr,
                     const size_t buffer_length  = 0) override;
  explicit ASSB(const cbxp_options_t& cbxp_options)
      : ControlBlock("assb", {}, cbxp_options, sizeof(struct assb)) {}
};

}  // namespace CBXP

#endif
