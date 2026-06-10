#include "ldax.hpp"

#include <cvt.h>
#include <ihaascb.h>
#include <ihaassb.h>
#include <ihaldax.h>
#include <ihapsa.h>

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "asvt.hpp"
#include "logger.hpp"

namespace CBXP {

nlohmann::json LDAX::get(const void* p_control_block,
                         const size_t buffer_length) {
  LDAX::checkDataLength(buffer_length);
  const struct ldax* p_ldax;
  nlohmann::json ldax_json = {};
  if (p_control_block == nullptr) {
    // PSA starts at address 0
    const struct psa* __ptr32 p_psa = 0;

    const struct cvtmap* __ptr32 p_cvtmap =
        // cppcheck-suppress nullPointer
        static_cast<struct cvtmap* __ptr32>(p_psa->flccvt);

    const asvt_t* __ptr32 p_asvt =
        static_cast<asvt_t* __ptr32>(p_cvtmap->cvtasvt);

    ldax_json["ldaxs"] = std::vector<nlohmann::json>();
    std::vector<nlohmann::json>& ldaxs =
        ldax_json["ldaxs"].get_ref<std::vector<nlohmann::json>&>();

    ldaxs.reserve(p_asvt->asvtmaxu);

    const uint32_t* __ptr32 p_ascb_addr =
        reinterpret_cast<const uint32_t* __ptr32>(&(p_asvt->asvtenty));

    for (int i = 0; i < p_asvt->asvtmaxu; i++) {
      if (0x80000000 & *p_ascb_addr) {
        Logger::getInstance().debug(formatter_.getHex<uint32_t>(p_ascb_addr) +
                                    " is not a valid ASCB address");
        p_ascb_addr++;
        continue;
      }

      // Cast ASCB address into ASCB pointer
      const struct ascb* __ptr32 p_ascb =
          reinterpret_cast<const struct ascb* __ptr32>(*p_ascb_addr);

      // Get ASSB from ASCB
      const struct assb* __ptr32 p_assb =
          reinterpret_cast<const struct assb* __ptr32>(p_ascb->ascbassb);

      // Get LDAX from ASSB
      // Get LDAX from ASSB - assbldax is a 64-bit pointer
      const struct ldax* p_next_ldax =
          // reinterpret_cast<const struct ldax*>(p_assb->assbldax);
          reinterpret_cast<const struct ldax*>(
              formatter_.uint<uint64_t>(p_assb->assbldax));

      nlohmann::json next_ldax =
          LDAX::get(*reinterpret_cast<const void* const*>(p_assb->assbldax));
      if (!next_ldax.is_null()) {
        ldaxs.push_back(next_ldax);
      }

      p_ascb_addr++;
    }

    return ldaxs;
  } else {
    p_ldax = static_cast<const struct ldax*>(p_control_block);
  }

  Logger::getInstance().debug("ldax hex dump:");
  Logger::getInstance().hexDump(reinterpret_cast<const char*>(p_ldax),
                                sizeof(struct ldax));

  ldax_json["ldax_id"] = formatter_.getString(p_ldax->ldax_id, 4);
  ldax_json["ldax_version"] =
      formatter_.getBitmap<uint8_t>(p_ldax->ldax_version);
  ldax_json["ldax_ldaascb"] =
      formatter_.getHex<uint32_t>(&(p_ldax->ldax_ldaascb));
  ldax_json["ldax_ldastrta"] =
      formatter_.getHex<uint32_t>(&(p_ldax->ldax_ldastrta));
  ldax_json["ldax_ldasiza"] = p_ldax->ldax_ldasiza;
  ldax_json["ldax_ldaestra"] =
      formatter_.getHex<uint32_t>(&(p_ldax->ldax_ldaestra));
  ldax_json["ldax_ldaesiza"] = p_ldax->ldax_ldaesiza;
  ldax_json["ldax_ldacrgtp"] =
      formatter_.getHex<uint32_t>(&(p_ldax->ldax_ldacrgtp));
  ldax_json["ldax_ldaergtp"] =
      formatter_.getHex<uint32_t>(&(p_ldax->ldax_ldaergtp));
  ldax_json["ldax_ldalimit"] =
      formatter_.getHex<uint32_t>(&(p_ldax->ldax_ldalimit));
  ldax_json["ldax_ldavvrg"] =
      formatter_.getHex<uint32_t>(&(p_ldax->ldax_ldavvrg));
  ldax_json["ldax_ldaelim"] =
      formatter_.getHex<uint32_t>(&(p_ldax->ldax_ldaelim));
  ldax_json["ldax_ldaevvrg"] =
      formatter_.getHex<uint32_t>(&(p_ldax->ldax_ldaevvrg));
  ldax_json["ldax_ldaloal"] =
      formatter_.getBitmap<uint32_t>(p_ldax->ldax_ldaloal);
  ldax_json["ldax_ldahial"] =
      formatter_.getBitmap<uint32_t>(p_ldax->ldax_ldahial);
  ldax_json["ldax_ldaeloal"] =
      formatter_.getBitmap<uint32_t>(p_ldax->ldax_ldaeloal);
  ldax_json["ldax_ldaehial"] =
      formatter_.getBitmap<uint32_t>(p_ldax->ldax_ldaehial);
  ldax_json["ldax_tcthwm"]  = p_ldax->ldax_tcthwm;
  ldax_json["ldax_tctlwm"]  = p_ldax->ldax_tctlwm;
  ldax_json["ldax_tctehwm"] = p_ldax->ldax_tctehwm;
  ldax_json["ldax_tctelwm"] = p_ldax->ldax_tctelwm;
  ldax_json["ldax_curhighbot"] =
      formatter_.getHex<uint32_t>(&(p_ldax->ldax_curhighbot));
  ldax_json["ldax_curehighbot"] =
      formatter_.getHex<uint32_t>(&(p_ldax->ldax_curehighbot));
  ldax_json["ldax_ldasmad"] =
      formatter_.getHex<uint32_t>(&(p_ldax->ldax_ldasmad));
  ldax_json["ldax_ldasmsz"] =
      formatter_.getHex<uint32_t>(&(p_ldax->ldax_ldasmsz));
  ldax_json["ldax_obtainshomespace"] = p_ldax->ldax_obtainshomespace;

  if (LDAX::matchFilter(ldax_json)) {
    return ldax_json;
  } else {
    return {};
  }
}

}  // namespace CBXP
