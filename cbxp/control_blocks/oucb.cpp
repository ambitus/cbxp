#include <cvt.h>
#include <ihaascb.h>
#include <ihaoucb.h>
#include <ihapsa.h>

#include <oucb.hpp>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "oucb.hpp"
#include "asvt.hpp"
#include "logger.hpp"

namespace CBXP {
nlohmann::json OUCB::get(void* __ptr32 p_control_block) {
  const oucb* __ptr32 p_oucb;
  nlohmann::json oucb_json = {};
  if (p_control_block == nullptr) {
    // PSA starts at address 0
    const struct psa* __ptr32 p_psa = 0;

    const struct cvtmap* __ptr32 p_cvtmap =
        // 'nullPointer' is a false positive because the PSA starts at address 0
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
      oucbs.push_back(
          oucb::get(reinterpret_cast<void* __ptr32>(p_ascb->ascboucb)));
      p_ascb_addr++;  // This SHOULD increment the pointer by 4 bytes.
    }
    return oucbs;
  } else {
    p_oucb = static_cast<oucb* __ptr32>(p_control_block);
  }

  Logger::getInstance().debug("oucb hex dump:");
  Logger::getInstance().hexDump(reinterpret_cast<const char*>(p_oucb),
                                sizeof(struct oucb));

  oucb_json["oucbname"] = formatter_.getString(p_oucb->oucbname, 4)


  return oucb_json;
}
}  // namespace CBXP
