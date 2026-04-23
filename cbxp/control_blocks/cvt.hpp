#ifndef __CVT_H_
#define __CVT_H_

#include <cvt.h>

#include "control_block.hpp"

namespace CBXP {

class CVT : public ControlBlock {
 public:
  nlohmann::json get(void* __ptr32 p_control_block = nullptr,
                     const uint64_t buffer_length  = -1) override;
  explicit CVT(const cbxp_options_t& cbxp_options)
      : ControlBlock("cvt", {"ecvt", "asvt"}, cbxp_options) {
    control_block_length = sizeof(struct cvtmap);
  }
};

}  // namespace CBXP

#endif
