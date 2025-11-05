#include "asvt.hpp"

#include <cvt.h>
#include <ihapsa.h>

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "ascb.hpp"
#include "asvt.hpp"
#include "logger.hpp"

namespace CBXP {
nlohmann::json ASVT::get(void* __ptr32 p_control_block) {
  const asvt_t* __ptr32 p_asvt;
  nlohmann::json asvt_json = {};

  if (p_control_block == nullptr) {
    const struct psa* __ptr32 p_psa = 0;
    const struct cvtmap* __ptr32 p_cvtmap =
        // 'nullPointer' is a false positive because the PSA starts at address 0
        // cppcheck-suppress nullPointer
        static_cast<struct cvtmap* __ptr32>(p_psa->flccvt);
    p_asvt = static_cast<asvt_t* __ptr32>(p_cvtmap->cvtasvt);
  } else {
    p_asvt = static_cast<asvt_t* __ptr32>(p_control_block);
  }

  Logger::getInstance().debug("ASCB pointers:");
  Logger::getInstance().hexDump(
      reinterpret_cast<const char*>(&p_asvt->asvtenty), p_asvt->asvtmaxu * 4);

  std::vector<std::string> ascbs;
  ascbs.reserve(p_asvt->asvtmaxu);
  const uint32_t* __ptr32 p_ascb = const_cast<uint32_t* __ptr32>(
      reinterpret_cast<const uint32_t* __ptr32>(&p_asvt->asvtenty));

  for (int i = 0; i < p_asvt->asvtmaxu; i++) {
    ascbs.push_back(formatter_.getHex<uint32_t>(p_ascb));
    p_ascb++;  // This SHOULD increment the pointer by 4 bytes.
  }

  asvt_json["asvtenty"] = ascbs;

  Logger::getInstance().debug("ASVT hex dump:");
  Logger::getInstance().hexDump(reinterpret_cast<const char*>(p_asvt),
                                sizeof(asvt_t));
  for (const auto& [include, include_includes] : include_map_) {
    if (include == "ascb") {
      nlohmann::json ascbs_json;
      CBXP::ASCB ascb(include_includes);
      uint32_t* __ptr32 p_ascb_addr = const_cast<uint32_t* __ptr32>(
          reinterpret_cast<const uint32_t* __ptr32>(&p_asvt->asvtenty));
      for (int i = 0; i < p_asvt->asvtmaxu; i++) {
        if (0x80000000 & *p_ascb_addr) {
          Logger::getInstance().debug(formatter_.getHex<uint32_t>(p_ascb_addr) +
                                      " is not a valid ASCB address");
          p_ascb_addr++;
          continue;
        }
        nlohmann::json ascb_json =
            ascb.get(reinterpret_cast<void*>(*p_ascb_addr));
        ascbs_json.push_back(ascb_json);
        p_ascb_addr++;  // This SHOULD increment the pointer by 4 bytes.
      }
      asvt_json["asvtenty"] = ascbs_json;
    }
  }

  // Get fields
  asvt_json["asvthwmasid"]     = p_asvt->asvthwmasid;
  asvt_json["asvtcurhighasid"] = p_asvt->asvtcurhighasid;
  asvt_json["asvtreua"]        = formatter_.getHex<uint32_t>(p_asvt->asvtreua);
  asvt_json["asvtravl"]        = formatter_.getHex<uint32_t>(p_asvt->asvtravl);
  asvt_json["asvtaav"]         = p_asvt->asvtaav;
  asvt_json["asvtast"]         = p_asvt->asvtast;
  asvt_json["asvtanr"]         = p_asvt->asvtanr;
  asvt_json["asvtstrt"]        = p_asvt->asvtstrt;
  asvt_json["asvtnonr"]        = p_asvt->asvtnonr;
  asvt_json["asvtmaxi"]        = p_asvt->asvtmaxi;
  asvt_json["asvtasvt"]        = formatter_.getString(p_asvt->asvtasvt, 4);
  asvt_json["asvtmaxu"]        = p_asvt->asvtmaxu;
  asvt_json["asvtmdsc"]        = p_asvt->asvtmdsc;
  asvt_json["asvtfrst"]        = formatter_.getHex<uint32_t>(p_asvt->asvtfrst);

  return asvt_json;
}
}  // namespace CBXP
