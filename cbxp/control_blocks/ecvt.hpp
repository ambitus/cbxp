#ifndef __ECVT_H_
#define __ECVT_H_

#include <ihaecvt.h>

#include "control_block.hpp"

namespace CBXP {

class ECVT : public ControlBlock {
 public:
  nlohmann::json get(void* p_control_block   = nullptr,
                     const int buffer_length = -1) override;
  explicit ECVT(const cbxp_options_t& cbxp_options)
      : ControlBlock("ecvt", {}, cbxp_options) {
    control_block_length_ = sizeof(struct ecvt);
  }
};
}  // namespace CBXP
#endif
