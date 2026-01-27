#ifndef __ASXB_H_
#define __ASXB_H_

#include "control_block.hpp"

namespace CBXP {

class ASXB : public ControlBlock {
 public:
  nlohmann::json get(void* __ptr32 p_control_block = nullptr) override;
  explicit ASXB(const std::vector<std::string>& includes)
      : ControlBlock("assb", {}, includes) {}
};

}  // namespace CBXP

#endif
