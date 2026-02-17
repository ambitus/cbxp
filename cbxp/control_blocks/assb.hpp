#ifndef __ASSB_H_
#define __ASSB_H_

#include "control_block.hpp"

namespace CBXP {

class ASSB : public ControlBlock {
 public:
  nlohmann::json get(void* __ptr32 p_control_block = nullptr) override;
  explicit ASSB(const cbxp_options_t& cbxp_options)
      : ControlBlock("assb", {}, cbxp_options) {}
};

}  // namespace CBXP

#endif
