#include "psa.hpp"

#include <ihapsa.h>

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

#include "cvt.hpp"
#include "logger.hpp"

namespace CBXP {
nlohmann::json PSA::get(void* __ptr32 p_control_block) {
  const struct psa* __ptr32 p_psa;
  nlohmann::json psa_json = {};

  if (p_control_block == nullptr) {
    // PSA starts at address 0
    p_psa = 0;
  } else {
    p_psa = static_cast<struct psa* __ptr32>(p_control_block);
  }

  Logger::getInstance().debug("PSA hex dump:");
  Logger::getInstance().hexDump(
      reinterpret_cast<const char*>(p_psa), sizeof(struct psa) / 2,
      true);  // Only the first "page" of the PSA is not fetch-protected

  psa_json["flccvt"] = formatter_.getHex<uint32_t>(&p_psa->flccvt);

  for (const auto& [include, include_includes] : include_map_) {
    if (include == "cvt") {
      psa_json["flccvt"] = CBXP::CVT(include_includes).get(p_psa->flccvt);
    }
  }

  // Get fields
  psa_json["psapsa"]   = formatter_.getString(p_psa->psapsa, 4);
  psa_json["flcsvilc"] = static_cast<int16_t>(p_psa->flcsvilc);
  psa_json["flcrnpsw_bitstring"] =
      formatter_.getBitmap<uint32_t>(p_psa->flcrnpsw);
  psa_json["flcrnpsw_hex"] = formatter_.getHex<uint32_t>(p_psa->flcrnpsw + 4);
  psa_json["flcsopsw"]     = formatter_.getPswSmall(p_psa->flcsopsw);
  psa_json["flcarch"]      = formatter_.getBitmap<uint8_t>(
      reinterpret_cast<const char*>(&p_psa->flcarch));
  psa_json["flccvt64"] = formatter_.getHex<uint64_t>(p_psa->flccvt64);
  psa_json["flcfacl"]  = formatter_.getBitmap<uint64_t>(p_psa->flcfacl) +
                        formatter_.getBitmap<uint64_t>(p_psa->flcfacl + 8);
  psa_json["flcfacle"] = formatter_.getBitmap<uint64_t>(p_psa->flcfacle) +
                         formatter_.getBitmap<uint64_t>(p_psa->flcfacle + 8);
  psa_json["psaaold"]  = formatter_.getHex<uint32_t>(p_psa->psaaold);
  psa_json["psaecvt"]  = formatter_.getHex<uint64_t>(p_psa->psaecvt);
  psa_json["psaflags"] = formatter_.getBitmap<uint8_t>(
      reinterpret_cast<const char*>(&p_psa->psaflags));
  psa_json["psafpfl"] = formatter_.getBitmap<uint8_t>(
      reinterpret_cast<const char*>(&p_psa->psafpfl));
  psa_json["psalaa"]   = formatter_.getHex<uint32_t>(p_psa->psalaa);
  psa_json["psalccav"] = formatter_.getHex<uint32_t>(&p_psa->psalccav);
  psa_json["psatold"]  = formatter_.getHex<uint32_t>(p_psa->psatold);
  psa_json["psatrvt"]  = formatter_.getHex<uint32_t>(p_psa->psatrvt);
  psa_json["psaval"]   = formatter_.getBitmap<uint16_t>(p_psa->psaval);
  psa_json["psaxcvt"]  = formatter_.getHex<uint64_t>(p_psa->psaxcvt);

  return psa_json;
}
}  // namespace CBXP
