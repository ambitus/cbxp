#ifndef __ASXB_H_
#define __ASXB_H_

#include "control_block.hpp"

namespace CBXP {

class ASXB : public ControlBlock {
 public:
  nlohmann::json get(void* __ptr32 p_control_block = nullptr) override;
  explicit ASSB(const std::vector<std::string>& includes)
      : ControlBlock("asxb", {"assb"}, includes) {}
};

}  // namespace CBXP

#endif
