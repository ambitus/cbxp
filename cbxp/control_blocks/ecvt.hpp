#ifndef __ECVT_H_
#define __ECVT_H_

#include <ihaecvt.h>

#include "control_block.hpp"

namespace CBXP {

class ECVT : public ControlBlock {
 public:
  nlohmann::json get(void* __ptr32 p_control_block = nullptr,
                     const uint64_t buffer_length  = -1) override;
  explicit ECVT(const cbxp_options_t& cbxp_options)
      : ControlBlock("ecvt", {}, cbxp_options) {
    control_block_length = sizeof(ecvt);
  }
};
}  // namespace CBXP
#endif
