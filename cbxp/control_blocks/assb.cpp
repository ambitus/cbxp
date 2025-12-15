#include "asvt.hpp"

#include <cvt.h>
#include <ihapsa.h>
#include <ihaascb.h>
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

  assb_json["assb_cms_lockinst_addr"] = formatter_.getHex<uint32_t>(&(p_assb->assb_cms_lockinst_addr));
  assb_json["assb_enqdeq_cms_lockinst_addr"] = formatter_.getHex<uint32_t>(&(p_assb->assb_enqdeq_cms_lockinst_addr));
  assb_json["assb_latch_cms_lockinst_addr"] = formatter_.getHex<uint32_t>(&(p_assb->assb_latch_cms_lockinst_addr));
  assb_json["assb_local_lockinst_addr"] = formatter_.getHex<uint32_t>(&(p_assb->assb_local_lockinst_addr));
  assb_json["assb_smfcms_lockinst_addr"] = formatter_.getHex<uint32_t>(&(p_assb->assb_smfcms_lockinst_addr));
  assb_json["assbdlcb"] = formatter_.getHex<uint32_t>(&(p_assb->assbdlcb));
  assb_json["assbmqma"] = formatter_.getHex<uint32_t>(&(p_assb->assbmqma));
  assb_json["assboasb"] = formatter_.getHex<uint32_t>(&(p_assb->assboasb));
  assb_json["assbtasb"] = formatter_.getHex<uint32_t>(&(p_assb->assbtasb));
  assb_json["assbvab"] = formatter_.getHex<uint32_t>(&(p_assb->assbvab));
  assb_json["assbldax"] = formatter_.getHex<uint32_t>(&(p_assb->assbldax));

  assb_json["assbisqn"] = p_assb->assbisqn;
  assb_json["assbjbni"] = formatter_.getString(p_assb->assbjbni, 8);
  assb_json["assbjbns"] = formatter_.getString(p_assb->assbjbns, 8);
  
  uint64_t assb_asst_time_on_cp;
  std::memcpy(&assb_asst_time_on_cp, p_assb->assb_asst_time_on_cp, 8);
  assb_json["assb_asst_time_on_cp"] = assb_asst_time_on_cp;

  uint64_t assb_asst_time_on_zcbp;
  std::memcpy(&assb_asst_time_on_zcbp, p_assb->assb_asst_time_on_zcbp, 8);
  assb_json["assb_asst_time_on_zcbp"] = assb_asst_time_on_zcbp;

  uint64_t assb_enct;
  std::memcpy(&assb_enct, p_assb->assb_enct, 8);
  assb_json["assb_enct"] = assb_enct;

  uint64_t assb_enct_prezos11;
  std::memcpy(&assb_enct_prezos11, p_assb->assb_enct_prezos11, 8);
  assb_json["assb_enct_prezos11"] = assb_enct_prezos11;

  uint64_t assb_ifa_enct;
  std::memcpy(&assb_ifa_enct, p_assb->assb_ifa_enct, 8);
  assb_json["assb_ifa_enct"] = assb_ifa_enct;

  uint64_t assb_ifa_enct_prezos11;
  std::memcpy(&assb_ifa_enct_prezos11, p_assb->assb_ifa_enct_prezos11, 8);
  assb_json["assb_ifa_enct_prezos11"] = assb_ifa_enct_prezos11;

  uint64_t assb_ifa_on_cp_enct;
  std::memcpy(&assb_ifa_on_cp_enct, p_assb->assb_ifa_on_cp_enct, 8);
  assb_json["assb_ifa_on_cp_enct"] = assb_ifa_on_cp_enct;

  uint64_t assb_ifa_phtm;
  std::memcpy(&assb_ifa_phtm, p_assb->assb_ifa_phtm, 8);
  assb_json["assb_ifa_phtm"] = assb_ifa_phtm;

  uint64_t assb_srb_time_on_cp;
  std::memcpy(&assb_srb_time_on_cp, p_assb->assb_srb_time_on_cp, 8);
  assb_json["assb_srb_time_on_cp"] = assb_srb_time_on_cp;

  uint64_t assb_srb_time_on_zcbp;
  std::memcpy(&assb_srb_time_on_zcbp, p_assb->assb_srb_time_on_zcbp, 8);
  assb_json["assb_srb_time_on_zcbp"] = assb_srb_time_on_zcbp;

  uint64_t assb_sup_on_cp_enct;
  std::memcpy(&assb_sup_on_cp_enct, p_assb->assb_sup_on_cp_enct, 8);
  assb_json["assb_sup_on_cp_enct"] = assb_sup_on_cp_enct;

  uint64_t assb_task_time_on_cp;
  std::memcpy(&assb_task_time_on_cp, p_assb->assb_task_time_on_cp, 8);
  assb_json["assb_task_time_on_cp"] = assb_task_time_on_cp;

  uint64_t assb_time_ifa_on_cp;
  std::memcpy(&assb_time_ifa_on_cp, p_assb->assb_time_ifa_on_cp, 8);
  assb_json["assb_time_ifa_on_cp"] = assb_time_ifa_on_cp;

  uint64_t assb_time_java_on_cp;
  std::memcpy(&assb_time_java_on_cp, p_assb->assb_time_java_on_cp, 8);
  assb_json["assb_time_java_on_cp"] = assb_time_java_on_cp;

  uint64_t assb_time_java_on_ziip;
  std::memcpy(&assb_time_java_on_ziip, p_assb->assb_time_java_on_ziip, 8);
  assb_json["assb_time_java_on_ziip"] = assb_time_java_on_ziip;

  uint64_t assb_time_on_ifa;
  std::memcpy(&assb_time_on_ifa, p_assb->assb_time_on_ifa, 8);
  assb_json["assb_time_on_ifa"] = assb_time_on_ifa;

  uint64_t assb_time_on_zaap;
  std::memcpy(&assb_time_on_zaap, p_assb->assb_time_on_zaap, 8);
  assb_json["assb_time_on_zaap"] = assb_time_on_zaap;

  uint64_t assb_time_on_zcbp;
  std::memcpy(&assb_time_on_zcbp, p_assb->assb_time_on_zcbp, 8);
  assb_json["assb_time_on_zcbp"] = assb_time_on_zcbp;

  uint64_t assb_time_on_ziip;
  std::memcpy(&assb_time_on_ziip, p_assb->assb_time_on_ziip, 8);
  assb_json["assb_time_on_ziip"] = assb_time_on_ziip;

  uint64_t assb_time_zcbp_on_cp;
  std::memcpy(&assb_time_zcbp_on_cp, p_assb->assb_time_zcbp_on_cp, 8);
  assb_json["assb_time_zcbp_on_cp"] = assb_time_zcbp_on_cp;

  uint64_t assb_time_ziip_on_cp;
  std::memcpy(&assb_time_ziip_on_cp, p_assb->assb_time_ziip_on_cp, 8);
  assb_json["assb_time_ziip_on_cp"] = assb_time_ziip_on_cp;

  uint64_t assb_zaap_enct;
  std::memcpy(&assb_zaap_enct, p_assb->assb_zaap_enct, 8);
  assb_json["assb_zaap_enct"] = assb_zaap_enct;

  uint64_t assb_zaap_phtm;
  std::memcpy(&assb_zaap_phtm, p_assb->assb_zaap_phtm, 8);
  assb_json["assb_zaap_phtm"] = assb_zaap_phtm;

  uint64_t assb_zcbp_base_phtm;
  std::memcpy(&assb_zcbp_base_phtm, p_assb->assb_zcbp_base_phtm, 8);
  assb_json["assb_zcbp_base_phtm"] = assb_zcbp_base_phtm;

  uint64_t assb_zcbp_enct;
  std::memcpy(&assb_zcbp_enct, p_assb->assb_zcbp_enct, 8);
  assb_json["assb_zcbp_enct"] = assb_zcbp_enct;

  uint64_t assb_zcbp_on_cp_enct;
  std::memcpy(&assb_zcbp_on_cp_enct, p_assb->assb_zcbp_on_cp_enct, 8);
  assb_json["assb_zcbp_on_cp_enct"] = assb_zcbp_on_cp_enct;

  uint64_t assb_zcbp_phtm;
  std::memcpy(&assb_zcbp_phtm, p_assb->assb_zcbp_phtm, 8);
  assb_json["assb_zcbp_phtm"] = assb_zcbp_phtm;

  uint64_t assb_ziip_enct;
  std::memcpy(&assb_ziip_enct, p_assb->assb_ziip_enct, 8);
  assb_json["assb_ziip_enct"] = assb_ziip_enct;

  uint64_t assb_ziip_on_cp_enct;
  std::memcpy(&assb_ziip_on_cp_enct, p_assb->assb_ziip_on_cp_enct, 8);
  assb_json["assb_ziip_on_cp_enct"] = assb_ziip_on_cp_enct;

  uint64_t assb_ziip_phtm;
  std::memcpy(&assb_ziip_phtm, p_assb->assb_ziip_phtm, 8);
  assb_json["assb_ziip_phtm"] = assb_ziip_phtm;

  uint64_t assb_ziip_phtm_base;
  std::memcpy(&assb_ziip_phtm_base, p_assb->assb_ziip_phtm_base, 8);
  assb_json["assb_ziip_phtm_base"] = assb_ziip_phtm_base;

  uint64_t assbasst;
  std::memcpy(&assbasst, p_assb->assbasst, 8);
  assb_json["assbasst"] = assbasst;

  uint64_t assbphtm;
  std::memcpy(&assbphtm, p_assb->assbphtm, 8);
  assb_json["assbphtm"] = assbphtm;

  uint64_t assbphtm_base;
  std::memcpy(&assbphtm_base, p_assb->assbphtm_base, 8);
  assb_json["assbphtm_base"] = assbphtm_base;
  uint64_t assbstkn;
  std::memcpy(&assbstkn, p_assb->assbstkn, 8);
  assb_json["assbstkn"] = assbstkn;

  uint64_t assbhst;
  std::memcpy(&assbhst, p_assb->assbhst, 8);
  assb_json["assbhst"] = assbhst;

  uint64_t assbiipt;
  std::memcpy(&assbiipt, p_assb->assbiipt, 8);
  assb_json["assbiipt"] = assbiipt;

  uint64_t assblasb;
  std::memcpy(&assblasb, p_assb->assblasb, 8);
  assb_json["assblasb"] = assblasb;

  uint64_t assb_time_on_cp;
  std::memcpy(&assb_time_on_cp, p_assb->assb_time_on_cp, 8);
  assb_json["assb_time_on_cp"] = assb_time_on_cp;

  uint64_t assb_base_phtm;
  std::memcpy(&assb_base_phtm, p_assb->assb_base_phtm, 8);
  assb_json["assb_base_phtm"] = assb_base_phtm;

  uint64_t assb_ifa_base_phtm;
  std::memcpy(&assb_ifa_base_phtm, p_assb->assb_ifa_base_phtm, 8);
  assb_json["assb_ifa_base_phtm"] = assb_ifa_base_phtm;

  uint64_t assb_base_enct;
  std::memcpy(&assb_base_enct, p_assb->assb_base_enct, 8);
  assb_json["assb_base_enct"] = assb_base_enct;

  uint64_t assb_zcbp_base_enct;
  std::memcpy(&assb_zcbp_base_enct, p_assb->assb_zcbp_base_enct, 8);
  assb_json["assb_zcbp_base_enct"] = assb_zcbp_base_enct;

  uint64_t assb_ifa_base_enct;
  std::memcpy(&assb_ifa_base_enct, p_assb->assb_ifa_base_enct, 8);
  assb_json["assb_ifa_base_enct"] = assb_ifa_base_enct;

  uint64_t assb_time_on_sup;
  std::memcpy(&assb_time_on_sup, p_assb->assb_time_on_sup, 8);
  assb_json["assb_time_on_sup"] = assb_time_on_sup;

  uint64_t assb_time_sup_on_cp;
  std::memcpy(&assb_time_sup_on_cp, p_assb->assb_time_sup_on_cp, 8);
  assb_json["assb_time_sup_on_cp"] = assb_time_sup_on_cp;

  uint64_t assb_sup_phtm;
  std::memcpy(&assb_sup_phtm, p_assb->assb_sup_phtm, 8);
  assb_json["assb_sup_phtm"] = assb_sup_phtm;

  uint64_t assb_sup_enct;
  std::memcpy(&assb_sup_enct, p_assb->assb_sup_enct, 8);
  assb_json["assb_sup_enct"] = assb_sup_enct;

  uint64_t assb_zcbp_on_cp_base_enct;
  std::memcpy(&assb_zcbp_on_cp_base_enct, p_assb->assb_zcbp_on_cp_base_enct, 8);
  assb_json["assb_zcbp_on_cp_base_enct"] = assb_zcbp_on_cp_base_enct;

  uint64_t assb_ifa_on_cp_base_enct;
  std::memcpy(&assb_ifa_on_cp_base_enct, p_assb->assb_ifa_on_cp_base_enct, 8);
  assb_json["assb_ifa_on_cp_base_enct"] = assb_ifa_on_cp_base_enct;

  uint64_t assb_time_at_pdp;
  std::memcpy(&assb_time_at_pdp, p_assb->assb_time_at_pdp, 8);
  assb_json["assb_time_at_pdp"] = assb_time_at_pdp;

  uint64_t assb_srbt_base;
  std::memcpy(&assb_srbt_base, p_assb->assb_srbt_base, 8);
  assb_json["assb_srbt_base"] = assb_srbt_base;

  uint64_t assb_switch_to_zaapziip_count;
  std::memcpy(&assb_switch_to_zaapziip_count, p_assb->assb_switch_to_zaapziip_count, 8);
  assb_json["assb_switch_to_zaapziip_count"] = assb_switch_to_zaapziip_count;

  uint64_t assbasab;
  std::memcpy(&assbasab, p_assb->assbasab, 8);
  assb_json["assbasab"] = assbasab;

  uint64_t assb_ziip_base_enct;
  std::memcpy(&assb_ziip_base_enct, p_assb->assb_ziip_base_enct, 8);
  assb_json["assb_ziip_base_enct"] = assb_ziip_base_enct;

  uint64_t assb_sup_base_enct;
  std::memcpy(&assb_sup_base_enct, p_assb->assb_sup_base_enct, 8);
  assb_json["assb_sup_base_enct"] = assb_sup_base_enct;

  uint64_t assb_ziip_on_cp_base_enct;
  std::memcpy(&assb_ziip_on_cp_base_enct, p_assb->assb_ziip_on_cp_base_enct, 8);
  assb_json["assb_ziip_on_cp_base_enct"] = assb_ziip_on_cp_base_enct;

  uint64_t assb_sup_on_cp_base_enct;
  std::memcpy(&assb_sup_on_cp_base_enct, p_assb->assb_sup_on_cp_base_enct, 8);
  assb_json["assb_sup_on_cp_base_enct"] = assb_sup_on_cp_base_enct;

  uint64_t assb_hdlockpromotion_time_at_pdp;
  std::memcpy(&assb_hdlockpromotion_time_at_pdp, p_assb->assb_hdlockpromotion_time_at_pdp, 8);
  assb_json["assb_hdlockpromotion_time_at_pdp"] = assb_hdlockpromotion_time_at_pdp;

  uint64_t assb_enct_hdlockpromote_time;
  std::memcpy(&assb_enct_hdlockpromote_time, p_assb->assb_enct_hdlockpromote_time, 8);
  assb_json["assb_enct_hdlockpromote_time"] = assb_enct_hdlockpromote_time;

  uint64_t assbsupc;
  std::memcpy(&assbsupc, p_assb->assbsupc, 8);
  assb_json["assbsupc"] = assbsupc;

  uint64_t assb_vartime_at_pdp;
  std::memcpy(&assb_vartime_at_pdp, p_assb->assb_vartime_at_pdp, 8);
  assb_json["assb_vartime_at_pdp"] = assb_vartime_at_pdp;

  uint64_t assb_varweighted_time_at_pdp;
  std::memcpy(&assb_varweighted_time_at_pdp, p_assb->assb_varweighted_time_at_pdp, 8);
  assb_json["assb_varweighted_time_at_pdp"] = assb_varweighted_time_at_pdp;

  uint64_t assb_scmbc;
  std::memcpy(&assb_scmbc, p_assb->assb_scmbc, 8);
  assb_json["assb_scmbc"] = assb_scmbc;

  uint64_t assbinitiatorjobid;
  std::memcpy(&assbinitiatorjobid, p_assb->assbinitiatorjobid, 8);
  assb_json["assbinitiatorjobid"] = assbinitiatorjobid;

  uint64_t assbend;
  std::memcpy(&assbend, p_assb->assbend, 8);
  assb_json["assbend"] = assbend;

  return assb_json;
}
}  // namespace CBXP
