#ifndef __ASCB_H_
#define __ASCB_H_

#include "control_block.hpp"

namespace CBXP {

class ASCB : public ControlBlock {
 public:
  nlohmann::json get(void* __ptr32 p_control_block = nullptr) override;
  explicit ASCB(const std::vector<std::string>& includes,
                const std::vector<std::string>& filters)
      : ControlBlock("ascb", {"assb"}, includes, filters) {}
};

}  // namespace CBXP

#endif
