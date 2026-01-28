#include <cvt.h>
#include <ihaascb.h>
#include <ihaasxb.h>
#include <ihapsa.h>

#include <asxb.hpp>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "ascb.hpp"
#include "asvt.hpp"
#include "logger.hpp"

namespace CBXP {
nlohmann::json ASXB::get(void* __ptr32 p_control_block) {
  const asxb* __ptr32 p_asxb;
  nlohmann::json asxb_json = {};
  if (p_control_block == nullptr) {
    // PSA starts at address 0
    const struct psa* __ptr32 p_psa = 0;

    const struct cvtmap* __ptr32 p_cvtmap =
        // 'nullPointer' is a false positive because the PSA starts at address 0
        // cppcheck-suppress nullPointer
        static_cast<struct cvtmap* __ptr32>(p_psa->flccvt);
    const asvt_t* __ptr32 p_asvt =
        static_cast<asvt_t* __ptr32>(p_cvtmap->cvtasvt);

    asxb_json["asxbs"] = std::vector<nlohmann::json>();
    std::vector<nlohmann::json>& asxbs =
        asxb_json["asxbs"].get_ref<std::vector<nlohmann::json>&>();
    asxbs.reserve(p_asvt->asvtmaxu);

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
      asxbs.push_back(
          ASXB::get(reinterpret_cast<void* __ptr32>(p_ascb->ascbasxb)));
      p_ascb_addr++;  // This SHOULD increment the pointer by 4 bytes.
    }
    return asxbs;
  } else {
    p_asxb = static_cast<asxb* __ptr32>(p_control_block);
  }

  Logger::getInstance().debug("asxb hex dump:");
  Logger::getInstance().hexDump(reinterpret_cast<const char*>(p_asxb),
                                sizeof(struct asxb));

    asxb_json["asxb_noabdump"] = p_asxb->asxb_noabdump;
    asxb_json["asxbftcb"] = formatter_.getHex<uint32_t>(&(p_asxb->asxbftcb));
    asxb_json["asxbitcb"] = formatter_.getHex<uint32_t>(&(p_asxb->asxbitcb));



  return asxb_json;
}
}  // namespace CBXP
