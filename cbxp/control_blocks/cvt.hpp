#ifndef __CVT_H_
#define __CVT_H_

#include "control_block.hpp"

namespace CBXP {

class CVT : public ControlBlock {
 public:
  nlohmann::json get(void* __ptr32 p_control_block = nullptr) override;
  explicit CVT(const cbxp_options_t& cbxp_options)
      : ControlBlock("cvt", {"ecvt", "asvt"}, cbxp_options) {}
};

}  // namespace CBXP

#endif
