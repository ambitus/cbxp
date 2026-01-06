#ifndef __CVT_H_
#define __CVT_H_

#include "control_block.hpp"

namespace CBXP {

class CVT : public ControlBlock {
 public:
  nlohmann::json get(void* __ptr32 p_control_block = nullptr) override;
  explicit CVT(const std::vector<std::string>& includes,
               const std::vector<std::string>& filters)
      : ControlBlock("cvt", {"ecvt", "asvt"}, includes, filters) {}
};

}  // namespace CBXP

#endif
