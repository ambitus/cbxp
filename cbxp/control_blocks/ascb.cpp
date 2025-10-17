#include "ascb.hpp"

#include <cvt.h>
#include <ihaascb.h>
#include <ihapsa.h>

#include <cstdint>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

#include "asvt.hpp"
#include "logger.hpp"

namespace CBXP {
nlohmann::json ASCB::get(void* __ptr32 p_control_block) {
  nlohmann::json ascb_json = {};
  const ascb* __ptr32 p_ascb;

  if (p_control_block == nullptr) {
    // PSA starts at address 0
    const struct psa* __ptr32 p_psa = 0;

    const struct cvtmap* __ptr32 p_cvtmap =
        // 'nullPointer' is a false positive because the PSA starts at address 0
        // cppcheck-suppress nullPointer
        static_cast<struct cvtmap* __ptr32>(p_psa->flccvt);
    asvt_t* __ptr32 p_asvt = static_cast<asvt_t* __ptr32>(p_cvtmap->cvtasvt);

    ascb_json["ascbs"]     = std::vector<nlohmann::json>();
    std::vector<nlohmann::json>& ascbs =
        ascb_json["ascbs"].get_ref<std::vector<nlohmann::json>&>();
    ascbs.reserve(p_asvt->asvtmaxu);

    uint32_t* __ptr32 p_ascb_addr =
        reinterpret_cast<uint32_t* __ptr32>(&p_asvt->asvtenty);
    for (int i = 0; i < p_asvt->asvtmaxu; i++) {
      if (0x80000000 & *p_ascb_addr) {
        Logger::getInstance().debug(formatter_.getHex<uint32_t>(p_ascb_addr) +
                                    " is not a valid ASCB address");
        p_ascb_addr++;
        continue;
      }
      ascbs.push_back(ASCB::get(reinterpret_cast<void* __ptr32>(*p_ascb_addr)));
      p_ascb_addr++;  // This SHOULD increment the pointer by 4 bytes.
    }
    return ascbs;
  } else {
    p_ascb = static_cast<ascb* __ptr32>(p_control_block);
  }

  ascb_json["ascbassb"] = formatter_.getHex<uint32_t>(&(p_ascb->ascbassb));
  ascb_json["ascbasxb"] = formatter_.getHex<uint32_t>(&(p_ascb->ascbasxb));

  Logger::getInstance().debug("ASCB hex dump:");
  Logger::getInstance().hexDump(reinterpret_cast<const char*>(p_ascb),
                                sizeof(struct ascb));

  ascb_json["ascbascb"] = formatter_.getString(p_ascb->ascbascb, 4);
  ascb_json["ascbasid"] = formatter_.getBitmap<uint32_t>(p_ascb->ascbasid);
  ascb_json["ascbdcti"] = p_ascb->ascbdcti;
  ascb_json["ascbejst"] = formatter_.getBitmap<uint64_t>(
      reinterpret_cast<const char*>(&p_ascb->ascbejst));
  ascb_json["ascbflg3"] = formatter_.getBitmap<uint32_t>(p_ascb->ascbflg3);
  ascb_json["ascbfw3"]  = formatter_.getBitmap<uint32_t>(
      reinterpret_cast<const char*>(&p_ascb->ascbfw3));
  ascb_json["ascbjbni"] = formatter_.getHex<uint32_t>(&(p_ascb->ascbjbni));
  ascb_json["ascbjbns"] = formatter_.getHex<uint32_t>(&(p_ascb->ascbjbns));
  ascb_json["ascblsqe"] = p_ascb->ascblsqe;
  ascb_json["ascblsqt"] = p_ascb->ascblsqt;
  ascb_json["ascbnoft"] = formatter_.getBitmap<uint32_t>(p_ascb->ascbnoft);
  ascb_json["ascboucb"] = formatter_.getHex<uint32_t>(&(p_ascb->ascboucb));
  ascb_json["ascbouxb"] = formatter_.getHex<uint32_t>(&(p_ascb->ascbouxb));
  ascb_json["ascbpo1m"] = formatter_.getBitmap<uint32_t>(p_ascb->ascbpo1m);
  ascb_json["ascbp1m0"] = formatter_.getBitmap<uint32_t>(p_ascb->ascbp1m0);
  ascb_json["ascbrsme"] = formatter_.getHex<uint32_t>(&(p_ascb->ascbrsme));
  ascb_json["ascbsdbf"] = formatter_.getBitmap<uint32_t>(p_ascb->ascbsdbf);
  ascb_json["ascbsrbt"] = formatter_.getBitmap<uint64_t>(
      reinterpret_cast<const char*>(&p_ascb->ascbsrbt));
  ascb_json["ascbtcbe"] = formatter_.getBitmap<uint32_t>(p_ascb->ascbtcbe);
  ascb_json["ascbtcbs"] = p_ascb->ascbtcbs;
  ascb_json["ascbxtcb"] = formatter_.getHex<uint32_t>(&(p_ascb->ascbxtcb));
  ascb_json["ascbzcx"]  = formatter_.getBitmap<uint32_t>(p_ascb->ascbzcx);

  return ascb_json;
}
}  // namespace CBXP
