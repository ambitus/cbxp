#ifndef __LDAX_H_
#define __LDAX_H_

#include <ihaldax.h>

#include "control_block.hpp"

namespace CBXP {

class LDAX : public ControlBlock {
 public:
  nlohmann::json get(const void* p_control_block = nullptr,
                     const size_t buffer_length  = 0) override;
  explicit LDAX(const cbxp_options_t& cbxp_options)
      : ControlBlock("ldax", {}, cbxp_options, sizeof(struct ldax)) {}
};

}  // namespace CBXP

#endif
