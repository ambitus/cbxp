#include <cvt.h>
#include <ihaascb.h>
#include <ihaassb.h>
#include <ihapsa.h>

#include <assb.hpp>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "ascb.hpp"
#include "asvt.hpp"
#include "logger.hpp"

namespace CBXP {
nlohmann::json ASSB::get(void* __ptr32 p_control_block) {
  const assb* __ptr32 p_assb;
  nlohmann::json assb_json = {};
  if (p_control_block == nullptr) {
    // PSA starts at address 0
    const struct psa* __ptr32 p_psa = 0;

    const struct cvtmap* __ptr32 p_cvtmap =
        // 'nullPointer' is a false positive because the PSA starts at address 0
        // cppcheck-suppress nullPointer
        static_cast<struct cvtmap* __ptr32>(p_psa->flccvt);
    const asvt_t* __ptr32 p_asvt =
        static_cast<asvt_t* __ptr32>(p_cvtmap->cvtasvt);

    assb_json["assbs"] = std::vector<nlohmann::json>();
    std::vector<nlohmann::json>& assbs =
        assb_json["assbs"].get_ref<std::vector<nlohmann::json>&>();
    assbs.reserve(p_asvt->asvtmaxu);

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
      assbs.push_back(
          ASSB::get(reinterpret_cast<void* __ptr32>(p_ascb->ascbassb)));
      p_ascb_addr++;  // This SHOULD increment the pointer by 4 bytes.
    }
    return assbs;
  } else {
    p_assb = static_cast<assb* __ptr32>(p_control_block);
  }

  Logger::getInstance().debug("assb hex dump:");
  Logger::getInstance().hexDump(reinterpret_cast<const char*>(p_assb),
                                sizeof(struct assb));

  assb_json["assb_cms_lockinst_addr"] =
      formatter_.getHex<uint32_t>(&(p_assb->assb_cms_lockinst_addr));
  assb_json["assb_enqdeq_cms_lockinst_addr"] =
      formatter_.getHex<uint32_t>(&(p_assb->assb_enqdeq_cms_lockinst_addr));
  assb_json["assb_latch_cms_lockinst_addr"] =
      formatter_.getHex<uint32_t>(&(p_assb->assb_latch_cms_lockinst_addr));
  assb_json["assb_local_lockinst_addr"] =
      formatter_.getHex<uint32_t>(&(p_assb->assb_local_lockinst_addr));
  assb_json["assb_smfcms_lockinst_addr"] =
      formatter_.getHex<uint32_t>(&(p_assb->assb_smfcms_lockinst_addr));
  assb_json["assbdlcb"] = formatter_.getHex<uint32_t>(&(p_assb->assbdlcb));
  assb_json["assbmqma"] = formatter_.getHex<uint32_t>(&(p_assb->assbmqma));
  assb_json["assboasb"] = formatter_.getHex<uint32_t>(&(p_assb->assboasb));
  assb_json["assbtasb"] = formatter_.getHex<uint32_t>(&(p_assb->assbtasb));
  assb_json["assbvab"]  = formatter_.getHex<uint32_t>(&(p_assb->assbvab));
  assb_json["assbldax"] = formatter_.getHex<uint32_t>(&(p_assb->assbldax));
  assb_json["assbisqn"] = p_assb->assbisqn;
  assb_json["assbjbni"] = formatter_.getString(p_assb->assbjbni, 8);
  assb_json["assbjbns"] = formatter_.getString(p_assb->assbjbns, 8);
  assb_json["assb_asst_time_on_cp"] =
      formatter_.uint<uint64_t>(p_assb->assb_asst_time_on_cp);
  assb_json["assb_asst_time_on_zcbp"] =
      formatter_.uint<uint64_t>(p_assb->assb_asst_time_on_zcbp);
  assb_json["assb_enct"] = formatter_.uint<uint64_t>(p_assb->assb_enct);
  assb_json["assb_enct_prezos11"] =
      formatter_.uint<uint64_t>(p_assb->assb_enct_prezos11);
  assb_json["assb_ifa_enct"] = formatter_.uint<uint64_t>(p_assb->assb_ifa_enct);
  assb_json["assb_ifa_enct_prezos11"] =
      formatter_.uint<uint64_t>(p_assb->assb_ifa_enct_prezos11);
  assb_json["assb_ifa_on_cp_enct"] =
      formatter_.uint<uint64_t>(p_assb->assb_ifa_on_cp_enct);
  assb_json["assb_ifa_phtm"] = formatter_.uint<uint64_t>(p_assb->assb_ifa_phtm);
  assb_json["assb_srb_time_on_cp"] =
      formatter_.uint<uint64_t>(p_assb->assb_srb_time_on_cp);
  assb_json["assb_srb_time_on_zcbp"] =
      formatter_.uint<uint64_t>(p_assb->assb_srb_time_on_zcbp);
  assb_json["assb_sup_on_cp_enct"] =
      formatter_.uint<uint64_t>(p_assb->assb_sup_on_cp_enct);
  assb_json["assb_task_time_on_cp"] =
      formatter_.uint<uint64_t>(p_assb->assb_task_time_on_cp);
  assb_json["assb_time_ifa_on_cp"] =
      formatter_.uint<uint64_t>(p_assb->assb_time_ifa_on_cp);
  assb_json["assb_time_java_on_cp"] =
      formatter_.uint<uint64_t>(p_assb->assb_time_java_on_cp);
  assb_json["assb_time_java_on_ziip"] =
      formatter_.uint<uint64_t>(p_assb->assb_time_java_on_ziip);
  assb_json["assb_time_on_ifa"] =
      formatter_.uint<uint64_t>(p_assb->assb_time_on_ifa);
  assb_json["assb_time_on_zaap"] =
      formatter_.uint<uint64_t>(p_assb->assb_time_on_zaap);
  assb_json["assb_time_on_zcbp"] =
      formatter_.uint<uint64_t>(p_assb->assb_time_on_zcbp);
  assb_json["assb_time_on_ziip"] =
      formatter_.uint<uint64_t>(p_assb->assb_time_on_ziip);
  assb_json["assb_time_zcbp_on_cp"] =
      formatter_.uint<uint64_t>(p_assb->assb_time_zcbp_on_cp);
  assb_json["assb_time_ziip_on_cp"] =
      formatter_.uint<uint64_t>(p_assb->assb_time_ziip_on_cp);
  assb_json["assb_zaap_enct"] =
      formatter_.uint<uint64_t>(p_assb->assb_zaap_enct);
  assb_json["assb_zaap_phtm"] =
      formatter_.uint<uint64_t>(p_assb->assb_zaap_phtm);
  assb_json["assb_zcbp_base_phtm"] =
      formatter_.uint<uint64_t>(p_assb->assb_zcbp_base_phtm);
  assb_json["assb_zcbp_enct"] =
      formatter_.uint<uint64_t>(p_assb->assb_zcbp_enct);
  assb_json["assb_zcbp_on_cp_enct"] =
      formatter_.uint<uint64_t>(p_assb->assb_zcbp_on_cp_enct);
  assb_json["assb_zcbp_phtm"] =
      formatter_.uint<uint64_t>(p_assb->assb_zcbp_phtm);
  assb_json["assb_ziip_enct"] =
      formatter_.uint<uint64_t>(p_assb->assb_ziip_enct);
  assb_json["assb_ziip_on_cp_enct"] =
      formatter_.uint<uint64_t>(p_assb->assb_ziip_on_cp_enct);
  assb_json["assb_ziip_phtm"] =
      formatter_.uint<uint64_t>(p_assb->assb_ziip_phtm);
  assb_json["assb_ziip_phtm_base"] =
      formatter_.uint<uint64_t>(p_assb->assb_ziip_phtm_base);
  assb_json["assbasst"]      = formatter_.uint<uint64_t>(p_assb->assbasst);
  assb_json["assbphtm"]      = formatter_.uint<uint64_t>(p_assb->assbphtm);
  assb_json["assbphtm_base"] = formatter_.uint<uint64_t>(p_assb->assbphtm_base);
  assb_json["assbstkn"]      = formatter_.uint<uint64_t>(p_assb->assbstkn);
  assb_json["assbhst"]       = formatter_.uint<uint64_t>(p_assb->assbhst);
  assb_json["assbiipt"]      = formatter_.uint<uint64_t>(p_assb->assbiipt);
  assb_json["assblasb"]      = formatter_.uint<uint64_t>(p_assb->assblasb);
  assb_json["assb_time_on_cp"] =
      formatter_.uint<uint64_t>(p_assb->assb_time_on_cp);
  assb_json["assb_base_phtm"] =
      formatter_.uint<uint64_t>(p_assb->assb_base_phtm);
  assb_json["assb_ifa_base_phtm"] =
      formatter_.uint<uint64_t>(p_assb->assb_ifa_base_phtm);
  assb_json["assb_base_enct"] =
      formatter_.uint<uint64_t>(p_assb->assb_base_enct);
  assb_json["assb_zcbp_base_enct"] =
      formatter_.uint<uint64_t>(p_assb->assb_zcbp_base_enct);
  assb_json["assb_ifa_base_enct"] =
      formatter_.uint<uint64_t>(p_assb->assb_ifa_base_enct);
  assb_json["assb_time_on_sup"] =
      formatter_.uint<uint64_t>(p_assb->assb_time_on_sup);
  assb_json["assb_time_sup_on_cp"] =
      formatter_.uint<uint64_t>(p_assb->assb_time_sup_on_cp);
  assb_json["assb_sup_phtm"] = formatter_.uint<uint64_t>(p_assb->assb_sup_phtm);
  assb_json["assb_sup_enct"] = formatter_.uint<uint64_t>(p_assb->assb_sup_enct);
  assb_json["assb_zcbp_on_cp_base_enct"] =
      formatter_.uint<uint64_t>(p_assb->assb_zcbp_on_cp_base_enct);
  assb_json["assb_ifa_on_cp_base_enct"] =
      formatter_.uint<uint64_t>(p_assb->assb_ifa_on_cp_base_enct);
  assb_json["assb_time_at_pdp"] =
      formatter_.uint<uint64_t>(p_assb->assb_time_at_pdp);
  assb_json["assb_srbt_base"] =
      formatter_.uint<uint64_t>(p_assb->assb_srbt_base);
  assb_json["assb_switch_to_zaapziip_count"] =
      formatter_.uint<uint64_t>(p_assb->assb_switch_to_zaapziip_count);
  assb_json["assbasab"] = formatter_.uint<uint64_t>(p_assb->assbasab);
  assb_json["assb_ziip_base_enct"] =
      formatter_.uint<uint64_t>(p_assb->assb_ziip_base_enct);
  assb_json["assb_sup_base_enct"] =
      formatter_.uint<uint64_t>(p_assb->assb_sup_base_enct);
  assb_json["assb_ziip_on_cp_base_enct"] =
      formatter_.uint<uint64_t>(p_assb->assb_ziip_on_cp_base_enct);
  assb_json["assb_sup_on_cp_base_enct"] =
      formatter_.uint<uint64_t>(p_assb->assb_sup_on_cp_base_enct);
  assb_json["assb_hdlockpromotion_time_at_pdp"] =
      formatter_.uint<uint64_t>(p_assb->assb_hdlockpromotion_time_at_pdp);
  assb_json["assb_enct_hdlockpromote_time"] =
      formatter_.uint<uint64_t>(p_assb->assb_enct_hdlockpromote_time);
  assb_json["assbsupc"] = formatter_.uint<uint64_t>(p_assb->assbsupc);
  assb_json["assb_vartime_at_pdp"] =
      formatter_.uint<uint64_t>(p_assb->assb_vartime_at_pdp);
  assb_json["assb_varweighted_time_at_pdp"] =
      formatter_.uint<uint64_t>(p_assb->assb_varweighted_time_at_pdp);
  assb_json["assb_scmbc"] = formatter_.uint<uint64_t>(p_assb->assb_scmbc);
  assb_json["assbinitiatorjobid"] =
      formatter_.uint<uint64_t>(p_assb->assbinitiatorjobid);
  assb_json["assbend"] = formatter_.uint<uint64_t>(p_assb->assbend);

  return assb_json;
}
}  // namespace CBXP
