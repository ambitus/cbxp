#ifndef __ASCB_H_
#define __ASCB_H_

#include "control_block.hpp"

namespace CBXP {

class ASCB : public ControlBlock {
 public:
  nlohmann::json get(void* __ptr32 p_control_block = nullptr) override;
  explicit ASCB(const cbxp_options_t& cbxp_options)
      : ControlBlock("ascb", {"assb", "oucb"}, cbxp_options) {}
};

}  // namespace CBXP

#endif
