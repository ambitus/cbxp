#ifndef __ECVT_H_
#define __ECVT_H_

#include <ihaecvt.h>

#include "control_block.hpp"

namespace CBXP {

class ECVT : public ControlBlock {
 public:
  nlohmann::json get(const void* p_control_block = nullptr,
                     const size_t buffer_length  = 0) override;
  explicit ECVT(const cbxp_options_t& cbxp_options)
      : ControlBlock("ecvt", {}, cbxp_options, sizeof(struct ecvt)) {}
};
}  // namespace CBXP
#endif
