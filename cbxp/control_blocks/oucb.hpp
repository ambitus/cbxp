#ifndef __OUCB_H_
#define __OUCB_H_

#include "control_block.hpp"


#pragma pack(push, 1)  // Don't byte align structure members.
typedef struct {
    char oucbname[4];
    char* __ptr32 oucbfwd;
    char* __ptr32 oucbbck;
    int32_t oucbtma;
    uint8_t oucbqfl;
    uint8_t oucbsfl;
    uint8_t oucbyfl;
    uint8_t oucbafl;
    uint8_t  oucbtfl;
    uint8_t oucbefl;
    uint8_t oucbasstatus;
    uint8_t oucbufl;
    uint8_t  oucblfl;
    uint8_t  oucbrfl;
    uint8_t  oucbndp;
    uint8_t  oucbtndp;
    uint8_t  oucbmfl;
    int8_t oucbiac;
    int8_t oucbrsv1;
    int8_t oucbpgp;
    //int8_t oucbwmg; //need to bring up with team
    int16_t oucbwsci;
    int16_t oucbwrci;
    uint8_t oucbmfl2;
    uint8_t oucbmfl3;
    int16_t oucbdmo;
    int8_t oucbdmn;
    int8_t oucbsrc;
    int16_t oucbswc;
    char* __ptr32 oucbascb;
    char* __ptr32 oucbpagp;
    int32_t oucbtmw;
    int32_t oucbwms;
    int32_t oucbcpu;
    int32_t oucbioc;
    int32_t oucbmso;
    int32_t oucbtms;
    int32_t oucbtmo;
    int32_t oucbdrfr;
    char* __ptr32 oucbact;
    int32_t oucbcsw; // speak to team
    union {
        uint32_t word;  
        struct {
            uint16_t oucbacn;  
            uint8_t  oucbcfl;  
            uint8_t  oucbcsbt; 
        } fields;
    } oucbcsw;
    int32_t oucbcmrv;
    int32_t oucbwmrl;
    int16_t  oucbval;
    uint8_t  oucbpfl;
    int8_t  oucbactl;
    int64_t oucbiocl;
    uint8_t oucbdspc;
    uint8_t  oucbdspn;
    int16_t  oucbntsp;
    int32_t  oucbps1;
    int32_t  oucbps2;
    int32_t oucbpst;
    //OUCBCHE2
    int32_t oucbrct;
    int32_t  oucbiit;
    int16_t  oucbnds;
    uint8_t oucbntsg;
    int8_t  oucbrsv2;
    int32_t  oucbtme;
    int32_t   oucbtml;
    int32_t   oucbdwms;
    int32_t  oucbsrb;
    int32_t  oucbtwss;
    int32_t  oucbtmp;
    int32_t  oucbdlyt;
    int32_t  oucbhst;
    int32_t  oucbcfs;
    char oucbsubn[4];
    int16_t  oucbrpg;
    int16_t oucbspg;
    int16_t oucbnpg;
    int16_t oucbsrpg;
    int16_t oucbnrpg;
    int16_t oucburpg;
    int16_t oucbcrpg;
    int16_t oucbarpg;
    int32_t oucbdrfp;
    char oucbtrxn[8];
    char  oucbusrd[8];
    char oucbcls[8];
    int32_t oucbtrs;
    int32_t  oucbtrr;
    int32_t oucbactp;
    int32_t oucbswss;
    int32_t oucbpsum;
    int16_t oucbfixb;
    uint8_t oucbaplv;
    uint8_t oucbesap;
    int32_t oucbrst1;
    int32_t oucbrst2;
    int32_t oucbx1_0;

} oucb_t;
#pragma pack(pop)  // Restore default structure packing options.


namespace CBXP {

class OUCB : public ControlBlock {
 public:
  nlohmann::json get(void* __ptr32 p_control_block = nullptr) override;
  explicit OUCB(const std::vector<std::string>& includes)
      : ControlBlock("oucb", {}, includes) {}
};

}  // namespace CBXP

#endif
