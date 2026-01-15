#ifndef __ECVT_H_
#define __ECVT_H_

#include "control_block.hpp"

namespace CBXP {

class ECVT : public ControlBlock {
 public:
  nlohmann::json get(void* __ptr32 p_control_block = nullptr) override;
  explicit ECVT(const cbxp_options_t& cbxp_options)
      : ControlBlock("ecvt", {}, cbxp_options) {}
};
}  // namespace CBXP
#endif
