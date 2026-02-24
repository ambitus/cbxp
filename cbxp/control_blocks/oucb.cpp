#include <cvt.h>
#include <ihaascb.h>
#include <ihapsa.h>

#include <cstdint>
#include <nlohmann/json.hpp>
#include <oucb.hpp>
#include <string>
#include <vector>

#include "asvt.hpp"
#include "logger.hpp"

namespace CBXP {
nlohmann::json OUCB::get(void* __ptr32 p_control_block) {
  const oucb_t* __ptr32 p_oucb;
  nlohmann::json oucb_json = {};
  if (p_control_block == nullptr) {
    // PSA starts at address 0
    const struct psa* __ptr32 p_psa = 0;

    const struct cvtmap* __ptr32 p_cvtmap =
        // 'nullPointer' is a false  positive because the PSA starts at address
        // cppcheck-suppress nullPointer
        static_cast<struct cvtmap* __ptr32>(p_psa->flccvt);
    const asvt_t* __ptr32 p_asvt =
        static_cast<asvt_t* __ptr32>(p_cvtmap->cvtasvt);

    oucb_json["oucbs"] = std::vector<nlohmann::json>();
    std::vector<nlohmann::json>& oucbs =
        oucb_json["oucbs"].get_ref<std::vector<nlohmann::json>&>();
    oucbs.reserve(p_asvt->asvtmaxu);

    const uint32_t* __ptr32 p_ascb_addr =
        reinterpret_cast<const uint32_t* __ptr32>(&p_asvt->asvtenty);
    for (int i = 0; i < p_asvt->asvtmaxu; i++) {
      if (0x80000000 & *p_ascb_addr) {
        Logger::getInstance().debug(formatter_.getHex<uint32_t>(p_ascb_addr) +
                                    " is not a valid ASCB address");
        p_ascb_addr++;
        continue;
      }
      // cast ascb addr into ascb pointer a
      //
      const struct ascb* __ptr32 p_ascb =
          reinterpret_cast<struct ascb* __ptr32>(*p_ascb_addr);
      nlohmann::json next_oucb =
          OUCB::get(reinterpret_cast<void* __ptr32>(p_ascb->ascboucb));
      if (!next_oucb.is_null()) {
        oucbs.push_back(next_oucb);
      }
      p_ascb_addr++;  // This SHOULD increment the pointer by 4 bytes.
    }
    return oucbs;
  } else {
    p_oucb = static_cast<oucb_t* __ptr32>(p_control_block);
  }

  Logger::getInstance().debug("oucb hex dump:");
  Logger::getInstance().hexDump(reinterpret_cast<const char*>(p_oucb),
                                sizeof(oucb_t));

  oucb_json["oucbname"] = formatter_.getString(p_oucb->oucbname, 4);
  oucb_json["oucbfwd"]  = formatter_.getHex<uint32_t>(&(p_oucb->oucbfwd));
  oucb_json["oucbbck"]  = formatter_.getHex<uint32_t>(&(p_oucb->oucbbck));
  oucb_json["oucbtma"]  = p_oucb->oucbtma;
  oucb_json["oucbqfl"]  = formatter_.getBitmap<uint8_t>(p_oucb->oucbqfl);
  oucb_json["oucbsfl"]  = formatter_.getBitmap<uint8_t>(p_oucb->oucbsfl);
  oucb_json["oucbyfl"]  = formatter_.getBitmap<uint8_t>(p_oucb->oucbyfl);
  oucb_json["oucbafl"]  = formatter_.getBitmap<uint8_t>(p_oucb->oucbafl);
  oucb_json["oucbtfl"]  = formatter_.getBitmap<uint8_t>(p_oucb->oucbtfl);
  oucb_json["oucbefl"]  = formatter_.getBitmap<uint8_t>(p_oucb->oucbefl);
  oucb_json["oucbasstatus"] =
      formatter_.getBitmap<uint8_t>(p_oucb->oucbasstatus);
  oucb_json["oucbufl"]  = formatter_.getBitmap<uint8_t>(p_oucb->oucbufl);
  oucb_json["oucblfl"]  = formatter_.getBitmap<uint8_t>(p_oucb->oucblfl);
  oucb_json["oucbrfl"]  = formatter_.getBitmap<uint8_t>(p_oucb->oucbrfl);
  oucb_json["oucbndp"]  = formatter_.getBitmap<uint8_t>(p_oucb->oucbndp);
  oucb_json["oucbtndp"] = formatter_.getBitmap<uint8_t>(p_oucb->oucbtndp);
  oucb_json["oucbmfl"]  = formatter_.getBitmap<uint8_t>(p_oucb->oucbmfl);
  oucb_json["oucbiac"]  = p_oucb->oucbiac;
  oucb_json["oucbrsv1"] = p_oucb->oucbrsv1;
  oucb_json["oucbpgp"]  = p_oucb->oucbpgp;
  oucb_json["oucbwsci"] = p_oucb->oucbwsci;
  oucb_json["oucbwrci"] = p_oucb->oucbwrci;
  oucb_json["oucbmfl2"] = formatter_.getBitmap<uint8_t>(p_oucb->oucbmfl2);
  oucb_json["oucbmfl3"] = formatter_.getBitmap<uint8_t>(p_oucb->oucbmfl3);
  oucb_json["oucbdmo"]  = p_oucb->oucbdmo;
  oucb_json["oucbdmn"]  = p_oucb->oucbdmn;
  oucb_json["oucbsrc"]  = p_oucb->oucbsrc;
  oucb_json["oucbswc"]  = p_oucb->oucbswc;
  oucb_json["oucbtmw"]  = p_oucb->oucbtmw;
  oucb_json["oucbwms"]  = p_oucb->oucbwms;
  oucb_json["oucbcpu"]  = p_oucb->oucbcpu;
  oucb_json["oucbioc"]  = p_oucb->oucbioc;
  oucb_json["oucbmso"]  = p_oucb->oucbmso;
  oucb_json["oucbtms"]  = p_oucb->oucbtms;
  oucb_json["oucbtmo"]  = p_oucb->oucbtmo;
  oucb_json["oucbdrfr"] = p_oucb->oucbdrfr;
  oucb_json["oucbact"]  = formatter_.getHex<uint32_t>(&(p_oucb->oucbact));
  // Union
  oucb_json["oucbcsw"]  = p_oucb->oucbcsw;
  oucb_json["oucbacn"]  = formatter_.getBitmap<uint16_t>(p_oucb->oucbacn);
  oucb_json["oucbcfl"]  = formatter_.getBitmap<uint8_t>(p_oucb->oucbcfl);
  oucb_json["oucbcsbt"] = formatter_.getBitmap<uint8_t>(p_oucb->oucbcsbt);
  oucb_json["oucbcmrv"] = p_oucb->oucbcmrv;
  oucb_json["oucbwmrl"] = p_oucb->oucbwmrl;
  oucb_json["oucbval"]  = p_oucb->oucbval;
  oucb_json["oucbpfl"]  = formatter_.getBitmap<uint8_t>(p_oucb->oucbpfl);
  oucb_json["oucbiocl"] = p_oucb->oucbiocl;
  oucb_json["oucbdspc"] = formatter_.getBitmap<uint8_t>(p_oucb->oucbdspc);
  oucb_json["oucbdspn"] = formatter_.getBitmap<uint8_t>(p_oucb->oucbdspn);
  oucb_json["oucbntsp"] = p_oucb->oucbntsp;
  oucb_json["oucbps1"]  = p_oucb->oucbps1;
  oucb_json["oucbps2"]  = p_oucb->oucbps2;
  oucb_json["oucbpst"]  = p_oucb->oucbpst;
  oucb_json["oucbrct"]  = p_oucb->oucbrct;
  oucb_json["oucbiit"]  = p_oucb->oucbiit;
  oucb_json["oucbnds"]  = p_oucb->oucbnds;
  oucb_json["oucbntsg"] = formatter_.getBitmap<uint8_t>(p_oucb->oucbntsg);
  oucb_json["oucbrsv2"] = p_oucb->oucbrsv2;
  oucb_json["oucbtme"]  = p_oucb->oucbtme;
  oucb_json["oucbtml"]  = p_oucb->oucbtml;
  oucb_json["oucbdwms"] = p_oucb->oucbdwms;
  oucb_json["oucbsrb"]  = p_oucb->oucbsrb;
  oucb_json["oucbtwss"] = p_oucb->oucbtwss;
  oucb_json["oucbtmp"]  = p_oucb->oucbtmp;
  oucb_json["oucbdlyt"] = p_oucb->oucbdlyt;
  oucb_json["oucbhst"]  = p_oucb->oucbhst;
  oucb_json["oucbcfs"]  = p_oucb->oucbcfs;
  oucb_json["oucbrpg"]  = p_oucb->oucbrpg;
  oucb_json["oucbspg"]  = p_oucb->oucbspg;
  oucb_json["oucbnpg"]  = p_oucb->oucbnpg;
  oucb_json["oucbsrpg"] = p_oucb->oucbsrpg;
  oucb_json["oucbnrpg"] = p_oucb->oucbnrpg;
  oucb_json["oucburpg"] = p_oucb->oucburpg;
  oucb_json["oucbcrpg"] = p_oucb->oucbcrpg;
  oucb_json["oucbarpg"] = p_oucb->oucbarpg;
  oucb_json["oucbdrfp"] = p_oucb->oucbdrfp;
  oucb_json["oucbtrxn"] = formatter_.getString(p_oucb->oucbtrxn, 8);
  oucb_json["oucbusrd"] = formatter_.getString(p_oucb->oucbusrd, 8);
  oucb_json["oucbcls"]  = formatter_.getString(p_oucb->oucbcls, 8);
  oucb_json["oucbtrs"]  = p_oucb->oucbtrs;
  oucb_json["oucbtrr"]  = p_oucb->oucbtrr;
  oucb_json["oucbswss"] = p_oucb->oucbswss;
  oucb_json["oucbpsum"] = p_oucb->oucbpsum;
  oucb_json["oucbfixb"] = p_oucb->oucbfixb;
  oucb_json["oucbaplv"] = formatter_.getBitmap<uint8_t>(p_oucb->oucbaplv);
  oucb_json["oucbesap"] = formatter_.getBitmap<uint8_t>(p_oucb->oucbesap);
  oucb_json["oucbrst1"] = p_oucb->oucbrst1;
  oucb_json["oucbrst2"] = p_oucb->oucbrst2;
  oucb_json["oucbx1_0"] = p_oucb->oucbx1_0;

  if (OUCB::matchFilter(oucb_json)) {
    return oucb_json;
  } else {
    return {};
  }
}
}  // namespace CBXP
