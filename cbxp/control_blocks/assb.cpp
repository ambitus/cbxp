#include "asvt.hpp"

#include <cvt.h>
#include <ihapsa.h>

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <ihaassb.h>
#include "ascb.hpp"
#include "asvt.hpp"
#include <assb.hpp>
#include "logger.hpp"


namespace CBXP {
nlohmann::json ASSB::get(void* __ptr32 p_control_block) {
const assb* __ptr32 p_assb;
const asvt_t* __ptr32 p_asvt;
  nlohmann::json assb_json = {};
  if (p_control_block == nullptr) {
    // PSA starts at address 0
    const struct psa* __ptr32 p_psa = 0;

    const struct cvtmap* __ptr32 p_cvtmap =
        // 'nullPointer' is a false positive because the PSA starts at address 0
        // cppcheck-suppress nullPointer
        static_cast<struct cvtmap* __ptr32>(p_psa->flccvt);
    asvt_t* __ptr32 p_asvt = static_cast<asvt_t* __ptr32>(p_cvtmap->cvtasvt);

    assb_json["assbs"]     = std::vector<nlohmann::json>();
    std::vector<nlohmann::json>& assbs =
        assb_json["assbs"].get_ref<std::vector<nlohmann::json>&>();
    assbs.reserve(p_asvt->asvtmaxu);

    uint32_t* __ptr32 p_ascb_addr =
        reinterpret_cast<uint32_t* __ptr32>(&p_asvt->asvtenty);
    for (int i = 0; i < p_asvt->asvtmaxu; i++) {
      if (0x80000000 & *p_ascb_addr) {
        Logger::getInstance().debug(formatter_.getHex<uint32_t>(p_ascb_addr) +
                                    " is not a valid ASCB address");
        p_ascb_addr++;
        continue;
      }
      // cast ascb addr into ascb pointer a
      // 
      const struct ascb* __ptr32 p_ascb = reinterpret_cast<struct ascb* __ptr32>(*p_ascb_addr);
      assbs.push_back(ASSB::get(reinterpret_cast<void* __ptr32>(p_ascb->ascbassb)));
      p_ascb_addr++;  // This SHOULD increment the pointer by 4 bytes.
    }
    return assbs;
  } else {
    p_assb = static_cast<assb* __ptr32>(p_control_block);

  }

  Logger::getInstance().debug("assb hex dump:");
  Logger::getInstance().hexDump(reinterpret_cast<const char*>(p_assb),
                                sizeof(struct assb));

  assb_json["assb_asst_time_on_cp"] = formatter_.getString(p_assb->assb_asst_time_on_cp, 8);;                    
  return assb_json;
}
}  // namespace CBXP
