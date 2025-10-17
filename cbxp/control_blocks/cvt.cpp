#include "cvt.hpp"

#include <cvt.h>
#include <ihapsa.h>

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

#include "asvt.hpp"
#include "ecvt.hpp"
#include "logger.hpp"

namespace CBXP {
nlohmann::json CVT::get(void* __ptr32 p_control_block) {
  const struct cvtmap* __ptr32 p_cvtmap;
  const struct cvtfix* __ptr32 p_cvtfix;
  const struct cvtxtnt2* __ptr32 p_cvtxtnt2;
  const struct cvtvstgx* __ptr32 p_cvtvstgx;
  nlohmann::json cvt_json = {};

  if (p_control_block == nullptr) {
    // PSA starts at address 0
    const struct psa* __ptr32 p_psa = 0;
    // 'nullPointer' is a false positive because the PSA starts at address 0
    // cppcheck-suppress-begin nullPointer
    p_cvtmap = static_cast<struct cvtmap* __ptr32>(p_psa->flccvt);
  } else {
    p_cvtmap = static_cast<struct cvtmap* __ptr32>(p_control_block);
  }
  p_cvtfix = const_cast<struct cvtfix* __ptr32>(
      reinterpret_cast<const struct cvtfix* __ptr32>(p_cvtmap));
  p_cvtxtnt2 = const_cast<struct cvtxtnt2* __ptr32>(
      reinterpret_cast<const struct cvtxtnt2* __ptr32>(p_cvtmap));
  p_cvtvstgx = const_cast<struct cvtvstgx* __ptr32>(
      reinterpret_cast<const struct cvtvstgx* __ptr32>(p_cvtmap));
  // cppcheck-suppress-end nullPointer

  Logger::getInstance().debug("CVT hex dump:");
  Logger::getInstance().hexDump(reinterpret_cast<const char*>(p_cvtmap),
                                sizeof(struct cvtmap));

  cvt_json["cvtasvt"] = formatter_.getHex<uint32_t>(&p_cvtmap->cvtasvt);
  cvt_json["cvtecvt"] = formatter_.getHex<uint32_t>(&p_cvtmap->cvtecvt);

  for (const auto& [include, include_includes] : include_map_) {
    if (include == "asvt") {
      cvt_json["cvtasvt"] = CBXP::ASVT(include_includes).get(p_cvtmap->cvtasvt);
    } else if (include == "ecvt") {
      cvt_json["cvtecvt"] = CBXP::ECVT(include_includes).get(p_cvtmap->cvtecvt);
    }
  }

  // Get fields

  cvt_json["cvtabend"] = formatter_.getHex<uint32_t>(p_cvtmap->cvtabend);
  cvt_json["cvtamff"]  = formatter_.getHex<uint32_t>(p_cvtmap->cvtamff);
  cvt_json["cvtasmvt"] = formatter_.getHex<uint32_t>(p_cvtmap->cvtasmvt);
  cvt_json["cvtbret"]  = formatter_.getHex<uint16_t>(p_cvtmap->cvtbret);
  cvt_json["cvtbsm0f"] = formatter_.getHex<uint16_t>(p_cvtmap->cvtbsm0f);
  cvt_json["cvtcsd"]   = formatter_.getHex<uint32_t>(p_cvtmap->cvtcsd);
  cvt_json["cvtctlfg"] = formatter_.getBitmap<uint8_t>(
      reinterpret_cast<const char*>(&p_cvtmap->cvtctlfg));
  cvt_json["cvtdcb"] = formatter_.getBitmap<uint8_t>(
      reinterpret_cast<const char*>(&p_cvtmap->cvtdcb));
  cvt_json["cvtdcpa"]  = formatter_.getBitmap<uint32_t>(p_cvtmap->cvtdcpa);
  cvt_json["cvtdfa"]   = formatter_.getHex<uint32_t>(p_cvtmap->cvtdfa);
  cvt_json["cvtedat2"] = formatter_.getBitmap<uint32_t>(p_cvtmap->cvtedat2);
  cvt_json["cvteplps"] = formatter_.getHex<uint32_t>(p_cvtvstgx->cvteplps);
  cvt_json["cvtexit"]  = formatter_.getHex<uint16_t>(p_cvtmap->cvtexit);
  cvt_json["cvtexp1"]  = formatter_.getHex<uint32_t>(p_cvtmap->cvtexp1);
  cvt_json["cvtflag2"] = formatter_.getBitmap<uint32_t>(p_cvtmap->cvtflag2);
  cvt_json["cvtflag3"] = formatter_.getBitmap<uint32_t>(p_cvtmap->cvtflag3);
  cvt_json["cvtflag4"] = formatter_.getBitmap<uint32_t>(p_cvtmap->cvtflag4);
  cvt_json["cvtflag5"] = formatter_.getBitmap<uint32_t>(p_cvtmap->cvtflag5);
  cvt_json["cvtflag6"] = formatter_.getBitmap<uint32_t>(p_cvtmap->cvtflag6);
  cvt_json["cvtflag7"] = formatter_.getBitmap<uint32_t>(p_cvtmap->cvtflag7);
  cvt_json["cvtflag9"] = formatter_.getBitmap<uint8_t>(
      reinterpret_cast<const char*>(&p_cvtmap->cvtflag9));
  cvt_json["cvtflgbt"] = formatter_.getBitmap<uint32_t>(p_cvtxtnt2->cvtflgbt);
  cvt_json["cvtgda"]   = formatter_.getHex<uint32_t>(p_cvtmap->cvtgda);
  cvt_json["cvtgrsst"] = formatter_.getBitmap<uint32_t>(p_cvtmap->cvtgrsst);
  cvt_json["cvtgvt"]   = formatter_.getHex<uint32_t>(p_cvtmap->cvtgvt);
  cvt_json["cvthid"]   = formatter_.getHex<uint32_t>(p_cvtmap->cvthid);
  cvt_json["cvtixavl"] = formatter_.getHex<uint32_t>(p_cvtmap->cvtixavl);
  cvt_json["cvtjesct"] = formatter_.getHex<uint32_t>(p_cvtmap->cvtjesct);
  cvt_json["cvtlccat"] = formatter_.getHex<uint32_t>(p_cvtmap->cvtlccat);
  cvt_json["cvtldto"]  = formatter_.getHex<uint64_t>(p_cvtxtnt2->cvtldto);
  cvt_json["cvtlink"]  = formatter_.getHex<uint32_t>(p_cvtmap->cvtlink);
  cvt_json["cvtlso"]   = formatter_.getHex<uint64_t>(p_cvtxtnt2->cvtlso);
  cvt_json["cvtmaxmp"] = p_cvtmap->cvtmaxmp;
  cvt_json["cvtmdl"]   = formatter_.getHex<uint16_t>(p_cvtfix->cvtmdl);
  cvt_json["cvtmser"]  = formatter_.getHex<uint32_t>(p_cvtmap->cvtmser);
  cvt_json["cvtopctp"] = formatter_.getHex<uint32_t>(p_cvtmap->cvtopctp);
  cvt_json["cvtoslvl"] = formatter_.getHex<uint64_t>(p_cvtmap->cvtoslvl) +
                         formatter_.getHex<uint64_t>(p_cvtmap->cvtoslvl + 8);
  cvt_json["cvtover"]  = formatter_.getBitmap<uint32_t>(p_cvtmap->cvtover);
  cvt_json["cvtpccat"] = formatter_.getHex<uint32_t>(p_cvtmap->cvtpccat);
  cvt_json["cvtpcnvt"] = formatter_.getHex<uint32_t>(p_cvtmap->cvtpcnvt);
  cvt_json["cvtprltv"] = formatter_.getHex<uint32_t>(p_cvtmap->cvtprltv);
  cvt_json["cvtprod"]  = formatter_.getHex<uint64_t>(p_cvtfix->cvtprod) +
                        formatter_.getHex<uint64_t>(p_cvtfix->cvtprod + 8);
  cvt_json["cvtpsxm"]  = formatter_.getHex<uint32_t>(p_cvtmap->cvtpsxm);
  cvt_json["cvtpvtp"]  = formatter_.getHex<uint32_t>(p_cvtmap->cvtpvtp);
  cvt_json["cvtqtd00"] = formatter_.getHex<uint32_t>(p_cvtmap->cvtqtd00);
  cvt_json["cvtqte00"] = formatter_.getHex<uint32_t>(p_cvtmap->cvtqte00);
  cvt_json["cvtrac"]   = formatter_.getHex<uint32_t>(p_cvtmap->cvtrac);
  cvt_json["cvtrcep"]  = formatter_.getHex<uint32_t>(p_cvtmap->cvtrcep);
  cvt_json["cvtrczrt"] = formatter_.getHex<uint32_t>(p_cvtmap->cvtrczrt);
  cvt_json["cvtrelno"] = formatter_.getHex<uint32_t>(p_cvtfix->cvtrelno);
  cvt_json["cvtri"]    = formatter_.getBitmap<uint32_t>(p_cvtmap->cvtri);
  cvt_json["cvtrtmct"] = formatter_.getHex<uint32_t>(p_cvtmap->cvtrtmct);
  cvt_json["cvtsaf"]   = formatter_.getHex<uint32_t>(p_cvtmap->cvtsaf);
  cvt_json["cvtscpin"] = formatter_.getHex<uint32_t>(p_cvtmap->cvtscpin);
  // cvt_json["cvtsdbf"] = formatter_.getBitmap<uint32_t>(p_cvtmap->cvtsdbf);
  cvt_json["cvtsdump"] = formatter_.getBitmap<uint32_t>(p_cvtmap->cvtsdump);
  cvt_json["cvtsmca"]  = formatter_.getHex<uint32_t>(p_cvtmap->cvtsmca);
  cvt_json["cvtsname"] = formatter_.getString(p_cvtmap->cvtsname, 8);
  cvt_json["cvtsubsp"] = formatter_.getBitmap<uint32_t>(p_cvtmap->cvtsubsp);
  cvt_json["cvtsvt"]   = formatter_.getHex<uint32_t>(p_cvtmap->cvtsvt);
  cvt_json["cvtsysad"] = formatter_.getHex<uint32_t>(p_cvtmap->cvtsysad);
  cvt_json["cvttpc"]   = formatter_.getHex<uint32_t>(p_cvtmap->cvttpc);
  cvt_json["cvttvt"]   = formatter_.getHex<uint32_t>(p_cvtmap->cvttvt);
  cvt_json["cvttx"]    = formatter_.getBitmap<uint32_t>(p_cvtmap->cvttx);
  cvt_json["cvttxc"]   = formatter_.getBitmap<uint32_t>(p_cvtmap->cvttxc);
  cvt_json["cvttxte"]  = formatter_.getBitmap<uint32_t>(p_cvtmap->cvttxte);
  cvt_json["cvttz"]    = p_cvtmap->cvttz;
  cvt_json["cvtucbsc"] = formatter_.getHex<uint32_t>(p_cvtmap->cvtucbsc);
  cvt_json["cvtundvm"] = formatter_.getBitmap<uint32_t>(p_cvtxtnt2->cvtundvm);
  cvt_json["cvtuser"]  = formatter_.getHex<uint32_t>(p_cvtmap->cvtuser);
  cvt_json["cvtverid"] = formatter_.getHex<uint64_t>(p_cvtfix->cvtverid);
  cvt_json["cvtvfget"] = formatter_.getHex<uint32_t>(p_cvtmap->cvtvfget);
  cvt_json["cvtvfind"] = formatter_.getHex<uint32_t>(p_cvtmap->cvtvfind);
  cvt_json["cvtvpsib"] = formatter_.getHex<uint32_t>(p_cvtmap->cvtvpsib);
  cvt_json["cvtvwait"] = formatter_.getHex<uint32_t>(p_cvtmap->cvtvwait);
  cvt_json["cvt0ef00"] = formatter_.getHex<uint32_t>(p_cvtmap->cvt0ef00);
  cvt_json["cvt0pt0e"] = formatter_.getHex<uint32_t>(p_cvtmap->cvt0pt0e);
  cvt_json["cvt0pt02"] = formatter_.getHex<uint32_t>(p_cvtmap->cvt0pt02);
  cvt_json["cvt0pt03"] = formatter_.getHex<uint32_t>(p_cvtmap->cvt0pt03);
  cvt_json["cvt0scr1"] = formatter_.getHex<uint32_t>(p_cvtmap->cvt0scr1);

  return cvt_json;
}
}  // namespace CBXP
