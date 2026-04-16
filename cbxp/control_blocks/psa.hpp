#ifndef __PSA_H_
#define __PSA_H_

#include <ihapsa.h>

#include "control_block.hpp"

namespace CBXP {

class PSA : public ControlBlock {
 public:
  nlohmann::json get(void* __ptr32 p_control_block = nullptr,
                     const uint64_t buffer_length  = -1) override;
  explicit PSA(const cbxp_options_t& cbxp_options)
      : ControlBlock("psa", {"cvt"}, cbxp_options) {
    control_block_length = sizeof(psa);
  }
};
}  // namespace CBXP
#endif
