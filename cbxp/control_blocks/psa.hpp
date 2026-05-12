#ifndef __PSA_H_
#define __PSA_H_

#include <ihapsa.h>

#include "control_block.hpp"

namespace CBXP {

class PSA : public ControlBlock {
 public:
  nlohmann::json get(void* p_control_block   = nullptr,
                     const int buffer_length = -1) override;
  explicit PSA(const cbxp_options_t& cbxp_options)
      : ControlBlock("psa", {"cvt"}, cbxp_options) {
    control_block_length_ = sizeof(struct psa) / 2;
  }
};
}  // namespace CBXP
#endif
