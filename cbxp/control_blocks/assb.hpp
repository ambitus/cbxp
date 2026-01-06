#ifndef __ASSB_H_
#define __ASSB_H_

#include "control_block.hpp"

namespace CBXP {

class ASSB : public ControlBlock {
 public:
  nlohmann::json get(void* __ptr32 p_control_block = nullptr) override;
  explicit ASSB(const std::vector<std::string>& includes,
                const std::vector<std::string>& filters)
      : ControlBlock("assb", {}, includes, filters) {}
};

}  // namespace CBXP

#endif
