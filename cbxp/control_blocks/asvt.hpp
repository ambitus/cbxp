#ifndef __ASVT_H_
#define __ASVT_H_

#include "control_block.hpp"

#pragma pack(push, 1)  // Don't byte align structure members.
typedef struct {
  char esvtprfx[464];
  int32_t asvthwmasid;
  int32_t asvtcurhighasid;
  char* __ptr32 asvtreua;
  char* __ptr32 asvtravl;
  int32_t asvtaav;
  int32_t asvtast;
  int32_t asvtanr;
  int32_t asvtstrt;
  int32_t asvtnonr;
  int32_t asvtmaxi;
  uint64_t reserved1;
  char asvtasvt[4];
  int32_t asvtmaxu;
  int32_t asvtmdsc;
  char* __ptr32 asvtfrst;
  // skipped this single bit
  char* __ptr32 asvtenty;

  // skipped ASVTAVAL ---> to bottom
} asvt_t;
#pragma pack(pop)  // Restore default structure packing options.

namespace CBXP {

class ASVT : public ControlBlock {
 public:
  nlohmann::json get(void* __ptr32 p_control_block = nullptr) override;
  explicit ASVT(const std::vector<std::string>& includes)
      : ControlBlock("asvt", {"ascb"}, includes) {}
};

}  // namespace CBXP

#endif
