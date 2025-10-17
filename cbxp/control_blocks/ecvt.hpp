#ifndef __ECVT_H_
#define __ECVT_H_

#include "control_block.hpp"

namespace CBXP {

class ECVT : public ControlBlock {
 public:
  nlohmann::json get(void* __ptr32 p_control_block = nullptr) override;
  explicit ECVT(const std::vector<std::string>& includes)
      : ControlBlock("ecvt", {}, includes) {}
};
}  // namespace CBXP
#endif
