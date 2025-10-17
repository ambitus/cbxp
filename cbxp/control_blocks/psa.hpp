#ifndef __PSA_H_
#define __PSA_H_

#include "control_block.hpp"

namespace CBXP {

class PSA : public ControlBlock {
 public:
  nlohmann::json get(void* __ptr32 p_control_block = nullptr) override;
  explicit PSA(const std::vector<std::string>& includes)
      : ControlBlock("psa", {"cvt"}, includes) {}
};
}  // namespace CBXP
#endif
