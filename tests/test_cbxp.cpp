#include <gtest/gtest.h>

#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "cbxp.h"

namespace {

// Helper to read binary sample files
std::vector<char> ReadSample(const std::string& filename) {
  // Relative to repository root or build output location
  std::string filepath = "tests/samples/" + filename;
  std::ifstream file(filepath, std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    // Try one directory up if run from build/tests directory
    filepath = "../tests/samples/" + filename;
    file.open(filepath, std::ios::binary | std::ios::ate);
  }
  if (!file.is_open()) {
    filepath = "samples/" + filename;
    file.open(filepath, std::ios::binary | std::ios::ate);
  }

  if (!file.is_open()) {
    return {};
  }

  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<char> buffer(size);
  file.read(buffer.data(), size);
  return buffer;
}

// Helper struct for RAII wrapper around cbxp_result_t
struct CBXPResultWrapper {
  cbxp_result_t* result = nullptr;

  CBXPResultWrapper(cbxp_result_t* res) : result(res) {}
  ~CBXPResultWrapper() {
    if (result != nullptr) {
      cbxp_free(result, false);
    }
  }

  cbxp_result_t* get() const { return result; }
  cbxp_result_t* operator->() const { return result; }
  bool isValid() const { return result != nullptr; }
};

// Helper function to extract and parse JSON
nlohmann::json ExtractJSON(const std::string& control_block,
                           const std::string& includes = "",
                           const std::string& filters  = "",
                           bool debug                  = false) {
  const char* p_includes = includes.empty() ? nullptr : includes.c_str();
  const size_t inc_len   = includes.length();
  const char* p_filters  = filters.empty() ? nullptr : filters.c_str();
  const size_t filt_len  = filters.length();

  CBXPResultWrapper res(cbxp_extract(control_block.c_str(),
                                     control_block.length(), p_includes,
                                     inc_len, p_filters, filt_len, debug));

  if (res.get() == nullptr || res->return_code != 0 ||
      res->result_json == nullptr) {
    return nullptr;
  }

  return nlohmann::json::parse(res->result_json, nullptr, false);
}

// Helper function to format and parse JSON
nlohmann::json FormatJSON(const std::string& control_block,
                          const std::vector<char>& data, bool debug = false) {
  CBXPResultWrapper res(cbxp_format(control_block.c_str(),
                                    control_block.length(), data.data(),
                                    data.size(), debug));

  if (res.get() == nullptr || res->return_code != 0 ||
      res->result_json == nullptr) {
    return nullptr;
  }

  return nlohmann::json::parse(res->result_json, nullptr, false);
}

// Helper to get cvtasmvt values
std::pair<uint64_t, std::string> GetCvtasmvtValues() {
  nlohmann::json cbdata = ExtractJSON("cvt");
  if (cbdata.is_null() || !cbdata.contains("cvtasmvt")) {
    return {0, ""};
  }
  std::string hex_str = cbdata["cvtasmvt"].get<std::string>();
  uint64_t int_val    = std::stoull(hex_str, nullptr, 16);
  return {int_val, hex_str};
}

}  // namespace

// ============================================================================
// Extract -- Basic Usage
// ============================================================================

TEST(TestCBXP, ExtractPSA) {
  CBXPResultWrapper res(cbxp_extract("psa", 3, nullptr, 0, nullptr, 0, false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 0);
  ASSERT_NE(res->result_json, nullptr);

  nlohmann::json cbdata = nlohmann::json::parse(res->result_json);
  EXPECT_TRUE(cbdata.is_object());
}

TEST(TestCBXP, ExtractCVT) {
  CBXPResultWrapper res(cbxp_extract("cvt", 3, nullptr, 0, nullptr, 0, false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 0);
  ASSERT_NE(res->result_json, nullptr);

  nlohmann::json cbdata = nlohmann::json::parse(res->result_json);
  EXPECT_TRUE(cbdata.is_object());
}

TEST(TestCBXP, ExtractECVT) {
  CBXPResultWrapper res(cbxp_extract("ecvt", 4, nullptr, 0, nullptr, 0, false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 0);
  ASSERT_NE(res->result_json, nullptr);

  nlohmann::json cbdata = nlohmann::json::parse(res->result_json);
  EXPECT_TRUE(cbdata.is_object());
}

TEST(TestCBXP, ExtractASVT) {
  CBXPResultWrapper res(cbxp_extract("asvt", 4, nullptr, 0, nullptr, 0, false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 0);
  ASSERT_NE(res->result_json, nullptr);

  nlohmann::json cbdata = nlohmann::json::parse(res->result_json);
  EXPECT_TRUE(cbdata.is_object());
}

TEST(TestCBXP, ExtractASCB) {
  CBXPResultWrapper res(cbxp_extract("ascb", 4, nullptr, 0, nullptr, 0, false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 0);
  ASSERT_NE(res->result_json, nullptr);

  nlohmann::json cbdata = nlohmann::json::parse(res->result_json);
  ASSERT_TRUE(cbdata.is_array());
  for (const auto& entry : cbdata) {
    EXPECT_TRUE(entry.is_object());
  }
}

TEST(TestCBXP, ExtractASSB) {
  CBXPResultWrapper res(cbxp_extract("assb", 4, nullptr, 0, nullptr, 0, false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 0);
  ASSERT_NE(res->result_json, nullptr);

  nlohmann::json cbdata = nlohmann::json::parse(res->result_json);
  ASSERT_TRUE(cbdata.is_array());
  for (const auto& entry : cbdata) {
    EXPECT_TRUE(entry.is_object());
  }
}

TEST(TestCBXP, ExtractOUCB) {
  CBXPResultWrapper res(cbxp_extract("oucb", 4, nullptr, 0, nullptr, 0, false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 0);
  ASSERT_NE(res->result_json, nullptr);

  nlohmann::json cbdata = nlohmann::json::parse(res->result_json);
  ASSERT_TRUE(cbdata.is_array());
  for (const auto& entry : cbdata) {
    EXPECT_TRUE(entry.is_object());
  }
}

TEST(TestCBXP, ExtractLDAX) {
  CBXPResultWrapper res(cbxp_extract("ldax", 4, nullptr, 0, nullptr, 0, false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 0);
  ASSERT_NE(res->result_json, nullptr);

  nlohmann::json cbdata = nlohmann::json::parse(res->result_json);
  ASSERT_TRUE(cbdata.is_array());
  for (const auto& entry : cbdata) {
    EXPECT_TRUE(entry.is_object());
  }
}

// ============================================================================
// Extract -- Debug Mode
// ============================================================================

TEST(TestCBXP, ExtractRunsInDebugMode) {
  CBXPResultWrapper res(cbxp_extract("psa", 3, nullptr, 0, nullptr, 0, true));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 0);
  ASSERT_NE(res->result_json, nullptr);

  nlohmann::json cbdata = nlohmann::json::parse(res->result_json);
  EXPECT_TRUE(cbdata.is_object());
}

// ============================================================================
// Extract -- Include Patterns
// ============================================================================

TEST(TestCBXP, ExtractPsaAndIncludeCvt) {
  nlohmann::json cbdata = ExtractJSON("psa", "cvt");
  ASSERT_TRUE(cbdata.is_object());
  EXPECT_TRUE(cbdata.contains("flccvt"));
  EXPECT_TRUE(cbdata["flccvt"].is_object());
}

TEST(TestCBXP, ExtractCvtAndIncludeEcvt) {
  nlohmann::json cbdata = ExtractJSON("cvt", "ecvt");
  ASSERT_TRUE(cbdata.is_object());
  EXPECT_TRUE(cbdata.contains("cvtecvt"));
  EXPECT_TRUE(cbdata["cvtecvt"].is_object());
}

TEST(TestCBXP, ExtractCvtAndIncludeAsvt) {
  nlohmann::json cbdata = ExtractJSON("cvt", "asvt");
  ASSERT_TRUE(cbdata.is_object());
  ASSERT_TRUE(cbdata.contains("cvtasvt"));
  EXPECT_TRUE(cbdata["cvtasvt"].is_object());
  ASSERT_TRUE(cbdata["cvtasvt"].contains("asvtenty"));
  EXPECT_TRUE(cbdata["cvtasvt"]["asvtenty"].is_array());
  for (const auto& entry : cbdata["cvtasvt"]["asvtenty"]) {
    EXPECT_TRUE(entry.is_string());
  }
}

TEST(TestCBXP, ExtractAsvtAndIncludeAscb) {
  nlohmann::json cbdata = ExtractJSON("asvt", "ascb");
  ASSERT_TRUE(cbdata.is_object());
  ASSERT_TRUE(cbdata.contains("asvtenty"));
  EXPECT_TRUE(cbdata["asvtenty"].is_array());
  for (const auto& entry : cbdata["asvtenty"]) {
    EXPECT_TRUE(entry.is_object());
  }
}

TEST(TestCBXP, ExtractAscbAndIncludeAssb) {
  nlohmann::json cbdata = ExtractJSON("ascb", "assb");
  ASSERT_TRUE(cbdata.is_array());
  for (const auto& entry : cbdata) {
    EXPECT_TRUE(entry.is_object());
    ASSERT_TRUE(entry.contains("ascbassb"));
    EXPECT_TRUE(entry["ascbassb"].is_object());
  }
}

TEST(TestCBXP, ExtractAscbAndIncludeOucb) {
  nlohmann::json cbdata = ExtractJSON("ascb", "oucb");
  ASSERT_TRUE(cbdata.is_array());
  for (const auto& entry : cbdata) {
    EXPECT_TRUE(entry.is_object());
    ASSERT_TRUE(entry.contains("ascboucb"));
    EXPECT_TRUE(entry["ascboucb"].is_object());
  }
}

TEST(TestCBXP, ExtractAsvtAndIncludeAscbAssbLdax) {
  nlohmann::json cbdata = ExtractJSON("asvt", "ascb.assb.ldax");
  ASSERT_TRUE(cbdata.is_object());
  ASSERT_TRUE(cbdata.contains("asvtenty"));
  EXPECT_TRUE(cbdata["asvtenty"].is_array());
  for (const auto& entry : cbdata["asvtenty"]) {
    EXPECT_TRUE(entry.is_object());
    ASSERT_TRUE(entry.contains("ascbassb"));
    EXPECT_TRUE(entry["ascbassb"].is_object());
    ASSERT_TRUE(entry["ascbassb"].contains("assbldax"));
    EXPECT_TRUE(entry["ascbassb"]["assbldax"].is_object());
  }
}

TEST(TestCBXP, ExtractPsaAndIncludeCvtAsvtAscbAssbLdax) {
  nlohmann::json cbdata = ExtractJSON("psa", "cvt.asvt.ascb.assb.ldax");
  ASSERT_TRUE(cbdata.is_object());
  ASSERT_TRUE(cbdata.contains("flccvt"));
  EXPECT_TRUE(cbdata["flccvt"].is_object());
  ASSERT_TRUE(cbdata["flccvt"].contains("cvtasvt"));
  EXPECT_TRUE(cbdata["flccvt"]["cvtasvt"].is_object());
  ASSERT_TRUE(cbdata["flccvt"]["cvtasvt"].contains("asvtenty"));
  EXPECT_TRUE(cbdata["flccvt"]["cvtasvt"]["asvtenty"].is_array());
  for (const auto& entry : cbdata["flccvt"]["cvtasvt"]["asvtenty"]) {
    EXPECT_TRUE(entry.is_object());
    ASSERT_TRUE(entry.contains("ascbassb"));
    EXPECT_TRUE(entry["ascbassb"].is_object());
    ASSERT_TRUE(entry["ascbassb"].contains("assbldax"));
    EXPECT_TRUE(entry["ascbassb"]["assbldax"].is_object());
  }
}

TEST(TestCBXP, ExtractAssbAndIncludeLdax) {
  nlohmann::json cbdata = ExtractJSON("assb", "ldax");
  ASSERT_TRUE(cbdata.is_array());
  for (const auto& entry : cbdata) {
    EXPECT_TRUE(entry.is_object());
    ASSERT_TRUE(entry.contains("assbldax"));
    EXPECT_TRUE(entry["assbldax"].is_object());
  }
}

TEST(TestCBXP, ExtractAscbAndIncludeAssbLdax) {
  nlohmann::json cbdata = ExtractJSON("ascb", "assb.ldax");
  ASSERT_TRUE(cbdata.is_array());
  for (const auto& entry : cbdata) {
    EXPECT_TRUE(entry.is_object());
    ASSERT_TRUE(entry.contains("ascbassb"));
    EXPECT_TRUE(entry["ascbassb"].is_object());
    ASSERT_TRUE(entry["ascbassb"].contains("assbldax"));
    EXPECT_TRUE(entry["ascbassb"]["assbldax"].is_object());
  }
}

TEST(TestCBXP, ExtractPsaAndIncludeCvtEcvt) {
  nlohmann::json cbdata = ExtractJSON("psa", "cvt.ecvt");
  ASSERT_TRUE(cbdata.is_object());
  ASSERT_TRUE(cbdata.contains("flccvt"));
  EXPECT_TRUE(cbdata["flccvt"].is_object());
  ASSERT_TRUE(cbdata["flccvt"].contains("cvtecvt"));
  EXPECT_TRUE(cbdata["flccvt"]["cvtecvt"].is_object());
}

TEST(TestCBXP, ExtractPsaAndIncludeCvtAsvtAscb) {
  nlohmann::json cbdata = ExtractJSON("psa", "cvt.asvt.ascb");
  ASSERT_TRUE(cbdata.is_object());
  ASSERT_TRUE(cbdata.contains("flccvt"));
  EXPECT_TRUE(cbdata["flccvt"].is_object());
  ASSERT_TRUE(cbdata["flccvt"].contains("cvtasvt"));
  EXPECT_TRUE(cbdata["flccvt"]["cvtasvt"].is_object());
  ASSERT_TRUE(cbdata["flccvt"]["cvtasvt"].contains("asvtenty"));
  EXPECT_TRUE(cbdata["flccvt"]["cvtasvt"]["asvtenty"].is_array());
  for (const auto& entry : cbdata["flccvt"]["cvtasvt"]["asvtenty"]) {
    EXPECT_TRUE(entry.is_object());
  }
}

TEST(TestCBXP, ExtractCvtAndIncludeAsvtAscb) {
  nlohmann::json cbdata = ExtractJSON("cvt", "asvt.ascb");
  ASSERT_TRUE(cbdata.is_object());
  ASSERT_TRUE(cbdata.contains("cvtasvt"));
  EXPECT_TRUE(cbdata["cvtasvt"].is_object());
  ASSERT_TRUE(cbdata["cvtasvt"].contains("asvtenty"));
  EXPECT_TRUE(cbdata["cvtasvt"]["asvtenty"].is_array());
  for (const auto& entry : cbdata["cvtasvt"]["asvtenty"]) {
    EXPECT_TRUE(entry.is_object());
  }
}

TEST(TestCBXP, ExtractCvtAndIncludeEcvtAndAsvt) {
  nlohmann::json cbdata = ExtractJSON("cvt", "ecvt,asvt");
  ASSERT_TRUE(cbdata.is_object());
  ASSERT_TRUE(cbdata.contains("cvtecvt"));
  EXPECT_TRUE(cbdata["cvtecvt"].is_object());
  ASSERT_TRUE(cbdata.contains("cvtasvt"));
  EXPECT_TRUE(cbdata["cvtasvt"].is_object());
  ASSERT_TRUE(cbdata["cvtasvt"].contains("asvtenty"));
  EXPECT_TRUE(cbdata["cvtasvt"]["asvtenty"].is_array());
  for (const auto& entry : cbdata["cvtasvt"]["asvtenty"]) {
    EXPECT_TRUE(entry.is_string());
  }
}

TEST(TestCBXP, ExtractPsaAndIncludeEcvtAsvtAndCvtAsvtAscb) {
  nlohmann::json cbdata = ExtractJSON("psa", "cvt.ecvt,cvt.asvt.ascb");
  ASSERT_TRUE(cbdata.is_object());
  ASSERT_TRUE(cbdata.contains("flccvt"));
  EXPECT_TRUE(cbdata["flccvt"].is_object());
  ASSERT_TRUE(cbdata["flccvt"].contains("cvtecvt"));
  EXPECT_TRUE(cbdata["flccvt"]["cvtecvt"].is_object());
  ASSERT_TRUE(cbdata["flccvt"].contains("cvtasvt"));
  EXPECT_TRUE(cbdata["flccvt"]["cvtasvt"].is_object());
  ASSERT_TRUE(cbdata["flccvt"]["cvtasvt"].contains("asvtenty"));
  EXPECT_TRUE(cbdata["flccvt"]["cvtasvt"]["asvtenty"].is_array());
  for (const auto& entry : cbdata["flccvt"]["cvtasvt"]["asvtenty"]) {
    EXPECT_TRUE(entry.is_object());
  }
}

TEST(TestCBXP, ExtractPsaAndIncludeEcvtAsvtAndCvtAsvtAscbAssb) {
  nlohmann::json cbdata = ExtractJSON("psa", "cvt.ecvt,cvt.asvt.ascb.assb");
  ASSERT_TRUE(cbdata.is_object());
  ASSERT_TRUE(cbdata.contains("flccvt"));
  EXPECT_TRUE(cbdata["flccvt"].is_object());
  ASSERT_TRUE(cbdata["flccvt"].contains("cvtecvt"));
  EXPECT_TRUE(cbdata["flccvt"]["cvtecvt"].is_object());
  ASSERT_TRUE(cbdata["flccvt"].contains("cvtasvt"));
  EXPECT_TRUE(cbdata["flccvt"]["cvtasvt"].is_object());
  ASSERT_TRUE(cbdata["flccvt"]["cvtasvt"].contains("asvtenty"));
  EXPECT_TRUE(cbdata["flccvt"]["cvtasvt"]["asvtenty"].is_array());
  for (const auto& entry : cbdata["flccvt"]["cvtasvt"]["asvtenty"]) {
    EXPECT_TRUE(entry.is_object());
    ASSERT_TRUE(entry.contains("ascbassb"));
    EXPECT_TRUE(entry["ascbassb"].is_object());
  }
}

TEST(TestCBXP, ExtractPsaAndIncludeEcvtAsvtAndCvtAsvtAscbOucb) {
  nlohmann::json cbdata = ExtractJSON("psa", "cvt.ecvt,cvt.asvt.ascb.oucb");
  ASSERT_TRUE(cbdata.is_object());
  ASSERT_TRUE(cbdata.contains("flccvt"));
  EXPECT_TRUE(cbdata["flccvt"].is_object());
  ASSERT_TRUE(cbdata["flccvt"].contains("cvtecvt"));
  EXPECT_TRUE(cbdata["flccvt"]["cvtecvt"].is_object());
  ASSERT_TRUE(cbdata["flccvt"].contains("cvtasvt"));
  EXPECT_TRUE(cbdata["flccvt"]["cvtasvt"].is_object());
  ASSERT_TRUE(cbdata["flccvt"]["cvtasvt"].contains("asvtenty"));
  EXPECT_TRUE(cbdata["flccvt"]["cvtasvt"]["asvtenty"].is_array());
  for (const auto& entry : cbdata["flccvt"]["cvtasvt"]["asvtenty"]) {
    EXPECT_TRUE(entry.is_object());
    ASSERT_TRUE(entry.contains("ascboucb"));
    EXPECT_TRUE(entry["ascboucb"].is_object());
  }
}

TEST(TestCBXP, ExtractPsaAndIncludeCvtRecursiveWildcard) {
  nlohmann::json cbdata = ExtractJSON("psa", "cvt.**");
  ASSERT_TRUE(cbdata.is_object());
  ASSERT_TRUE(cbdata.contains("flccvt"));
  EXPECT_TRUE(cbdata["flccvt"].is_object());
  ASSERT_TRUE(cbdata["flccvt"].contains("cvtecvt"));
  EXPECT_TRUE(cbdata["flccvt"]["cvtecvt"].is_object());
  ASSERT_TRUE(cbdata["flccvt"].contains("cvtasvt"));
  EXPECT_TRUE(cbdata["flccvt"]["cvtasvt"].is_object());
  ASSERT_TRUE(cbdata["flccvt"]["cvtasvt"].contains("asvtenty"));
  EXPECT_TRUE(cbdata["flccvt"]["cvtasvt"]["asvtenty"].is_array());
  for (const auto& entry : cbdata["flccvt"]["cvtasvt"]["asvtenty"]) {
    EXPECT_TRUE(entry.is_object());
    ASSERT_TRUE(entry.contains("ascbassb"));
    EXPECT_TRUE(entry["ascbassb"].is_object());
    ASSERT_TRUE(entry["ascbassb"].contains("assbldax"));
    EXPECT_TRUE(entry["ascbassb"]["assbldax"].is_object());
    ASSERT_TRUE(entry.contains("ascboucb"));
    EXPECT_TRUE(entry["ascboucb"].is_object());
  }
}

TEST(TestCBXP, ExtractPsaAndIncludeCvtWildcard) {
  nlohmann::json cbdata = ExtractJSON("psa", "cvt.*");
  ASSERT_TRUE(cbdata.is_object());
  ASSERT_TRUE(cbdata.contains("flccvt"));
  EXPECT_TRUE(cbdata["flccvt"].is_object());
  ASSERT_TRUE(cbdata["flccvt"].contains("cvtecvt"));
  EXPECT_TRUE(cbdata["flccvt"]["cvtecvt"].is_object());
  ASSERT_TRUE(cbdata["flccvt"].contains("cvtasvt"));
  EXPECT_TRUE(cbdata["flccvt"]["cvtasvt"].is_object());
  ASSERT_TRUE(cbdata["flccvt"]["cvtasvt"].contains("asvtenty"));
  EXPECT_TRUE(cbdata["flccvt"]["cvtasvt"]["asvtenty"].is_array());
  for (const auto& entry : cbdata["flccvt"]["cvtasvt"]["asvtenty"]) {
    EXPECT_TRUE(entry.is_string());
  }
}

TEST(TestCBXP, ExtractCvtAndIncludeWildcardAndAsvtWildcard) {
  nlohmann::json cbdata = ExtractJSON("cvt", "*,asvt.*");
  ASSERT_TRUE(cbdata.is_object());
  ASSERT_TRUE(cbdata.contains("cvtecvt"));
  EXPECT_TRUE(cbdata["cvtecvt"].is_object());
  ASSERT_TRUE(cbdata.contains("cvtasvt"));
  EXPECT_TRUE(cbdata["cvtasvt"].is_object());
  ASSERT_TRUE(cbdata["cvtasvt"].contains("asvtenty"));
  EXPECT_TRUE(cbdata["cvtasvt"]["asvtenty"].is_array());
  for (const auto& entry : cbdata["cvtasvt"]["asvtenty"]) {
    EXPECT_TRUE(entry.is_object());
  }
}

TEST(TestCBXP, ExtractCvtAndIncludeWildcardAndAsvtRecursiveWildcard) {
  nlohmann::json cbdata = ExtractJSON("cvt", "*,asvt.**");
  ASSERT_TRUE(cbdata.is_object());
  ASSERT_TRUE(cbdata.contains("cvtecvt"));
  EXPECT_TRUE(cbdata["cvtecvt"].is_object());
  ASSERT_TRUE(cbdata.contains("cvtasvt"));
  EXPECT_TRUE(cbdata["cvtasvt"].is_object());
  ASSERT_TRUE(cbdata["cvtasvt"].contains("asvtenty"));
  EXPECT_TRUE(cbdata["cvtasvt"]["asvtenty"].is_array());
  for (const auto& entry : cbdata["cvtasvt"]["asvtenty"]) {
    EXPECT_TRUE(entry.is_object());
    ASSERT_TRUE(entry.contains("ascbassb"));
    EXPECT_TRUE(entry["ascbassb"].is_object());
    ASSERT_TRUE(entry.contains("ascboucb"));
    EXPECT_TRUE(entry["ascboucb"].is_object());
  }
}

TEST(TestCBXP, ExtractAscbAndIncludeAssbRecursiveWildcard) {
  nlohmann::json cbdata = ExtractJSON("ascb", "assb.**");
  ASSERT_TRUE(cbdata.is_array());
  for (const auto& entry : cbdata) {
    EXPECT_TRUE(entry.is_object());
    ASSERT_TRUE(entry.contains("ascbassb"));
    EXPECT_TRUE(entry["ascbassb"].is_object());
    ASSERT_TRUE(entry["ascbassb"].contains("assbldax"));
    EXPECT_TRUE(entry["ascbassb"]["assbldax"].is_object());
  }
}

TEST(TestCBXP, ExtractAscbAndIncludeOucbAndAssbRecursiveWildcard) {
  nlohmann::json cbdata = ExtractJSON("ascb", "oucb,assb.**");
  ASSERT_TRUE(cbdata.is_array());
  for (const auto& entry : cbdata) {
    EXPECT_TRUE(entry.is_object());
    ASSERT_TRUE(entry.contains("ascbassb"));
    EXPECT_TRUE(entry["ascbassb"].is_object());
    ASSERT_TRUE(entry["ascbassb"].contains("assbldax"));
    EXPECT_TRUE(entry["ascbassb"]["assbldax"].is_object());
    ASSERT_TRUE(entry.contains("ascboucb"));
    EXPECT_TRUE(entry["ascboucb"].is_object());
  }
}

// ============================================================================
// Extract -- Filters
// ============================================================================

TEST(TestCBXP, ExtractPsaFilterEyecatcher) {
  nlohmann::json cbdata = ExtractJSON("psa", "", "psapsa=PSA");
  EXPECT_TRUE(cbdata.is_object());
}

TEST(TestCBXP, ExtractPsaFilterWildcardInclude) {
  nlohmann::json cbdata =
      ExtractJSON("psa", "**", "cvt.asvt.ascb.assb.assbjbns=*MASTER*");
  EXPECT_TRUE(cbdata.is_object());
}

TEST(TestCBXP, ExtractFilterWithExplicitInclude) {
  nlohmann::json cbdata = ExtractJSON("psa", "cvt.asvt.ascb.assb",
                                      "cvt.asvt.ascb.assb.assbjbns=*MASTER*");
  EXPECT_TRUE(cbdata.is_object());
}

TEST(TestCBXP, ExtractMultipleFilters) {
  nlohmann::json cbdata = ExtractJSON("psa", "cvt.asvt.ascb.assb",
                                      "cvt.asvt.ascb.assb.assbjbns=*MASTER*,"
                                      "cvt.asvt.ascb.ascbasid>0");
  EXPECT_TRUE(cbdata.is_object());
}

TEST(TestCBXP, ExtractWildcardFilterWithString) {
  nlohmann::json cbdata = ExtractJSON("psa", "cvt.asvt.ascb.assb",
                                      "cvt.asvt.ascb.assb.assbjbns=?MAS?ER?");
  EXPECT_TRUE(cbdata.is_object());
}

TEST(TestCBXP, ExtractIntFilterEqual) {
  nlohmann::json cbdata =
      ExtractJSON("psa", "cvt.asvt.ascb.assb", "cvt.asvt.ascb.ascbasid=1");
  EXPECT_TRUE(cbdata.is_object());
}

TEST(TestCBXP, ExtractIntFilterGreaterThan) {
  nlohmann::json cbdata =
      ExtractJSON("psa", "cvt.asvt.ascb.assb", "cvt.asvt.ascb.ascbasid>0");
  EXPECT_TRUE(cbdata.is_object());
}

TEST(TestCBXP, ExtractIntFilterLessThan) {
  nlohmann::json cbdata =
      ExtractJSON("psa", "cvt.asvt.ascb.assb", "cvt.asvt.ascb.ascbasid<2");
  EXPECT_TRUE(cbdata.is_object());
}

TEST(TestCBXP, ExtractIntFilterGreaterThanOrEqual) {
  nlohmann::json cbdata =
      ExtractJSON("psa", "cvt.asvt.ascb.assb", "cvt.asvt.ascb.ascbasid>=1");
  EXPECT_TRUE(cbdata.is_object());
}

TEST(TestCBXP, ExtractIntFilterLessThanOrEqual) {
  nlohmann::json cbdata =
      ExtractJSON("psa", "cvt.asvt.ascb.assb", "cvt.asvt.ascb.ascbasid<=2");
  EXPECT_TRUE(cbdata.is_object());
}

TEST(TestCBXP, ExtractIntFilterWithHexFieldEqual) {
  auto [cvtasmvt_int, _] = GetCvtasmvtValues();
  nlohmann::json cbdata =
      ExtractJSON("cvt", "", "cvtasmvt=" + std::to_string(cvtasmvt_int));
  EXPECT_TRUE(cbdata.is_object());
}

TEST(TestCBXP, ExtractHexFilterWithEqual) {
  auto [_, cvtasmvt_hex] = GetCvtasmvtValues();
  nlohmann::json cbdata  = ExtractJSON("cvt", "", "cvtasmvt=" + cvtasmvt_hex);
  EXPECT_TRUE(cbdata.is_object());
}

TEST(TestCBXP, ExtractHexFilterWithGreaterThan) {
  auto [cvtasmvt_int, _] = GetCvtasmvtValues();
  char hex_buf[32];
  snprintf(hex_buf, sizeof(hex_buf), "0x%llX",
           static_cast<unsigned long long>(cvtasmvt_int - 1));
  nlohmann::json cbdata =
      ExtractJSON("cvt", "", "cvtasmvt>" + std::string(hex_buf));
  EXPECT_TRUE(cbdata.is_object());
}

TEST(TestCBXP, ExtractHexFilterWithLessThan) {
  auto [cvtasmvt_int, _] = GetCvtasmvtValues();
  char hex_buf[32];
  snprintf(hex_buf, sizeof(hex_buf), "0x%llX",
           static_cast<unsigned long long>(cvtasmvt_int + 1));
  nlohmann::json cbdata =
      ExtractJSON("cvt", "", "cvtasmvt<" + std::string(hex_buf));
  EXPECT_TRUE(cbdata.is_object());
}

TEST(TestCBXP, ExtractHexFilterWithGreaterThanOrEqual) {
  auto [_, cvtasmvt_hex] = GetCvtasmvtValues();
  nlohmann::json cbdata  = ExtractJSON("cvt", "", "cvtasmvt>=" + cvtasmvt_hex);
  EXPECT_TRUE(cbdata.is_object());
}

TEST(TestCBXP, ExtractHexFilterWithLessThanOrEqual) {
  auto [_, cvtasmvt_hex] = GetCvtasmvtValues();
  nlohmann::json cbdata  = ExtractJSON("cvt", "", "cvtasmvt<=" + cvtasmvt_hex);
  EXPECT_TRUE(cbdata.is_object());
}

TEST(TestCBXP, ExtractReturnsNullIfNoFilterMatch) {
  CBXPResultWrapper res(
      cbxp_extract("psa", 3, nullptr, 0, "psapsa=PSB", 10, false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 0);
  ASSERT_NE(res->result_json, nullptr);
  EXPECT_STREQ(res->result_json, "null");
}

TEST(TestCBXP, ExtractReturnsNullIfOneOfTwoFiltersFails) {
  const std::string includes = "assb";
  const std::string filters  = "assb.assbjbns=*MASTER*,ascbasid>2";
  CBXPResultWrapper res(cbxp_extract("ascb", 4, includes.c_str(),
                                     includes.length(), filters.c_str(),
                                     filters.length(), false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 0);
  ASSERT_NE(res->result_json, nullptr);
  EXPECT_STREQ(res->result_json, "null");
}

TEST(TestCBXP, ExtractFilterOucbtrxnFromOucb) {
  nlohmann::json cbdata = ExtractJSON("oucb", "", "oucbtrxn=OMVS");
  ASSERT_TRUE(cbdata.is_array());
  for (const auto& entry : cbdata) {
    EXPECT_TRUE(entry.is_object());
    ASSERT_TRUE(entry.contains("oucbtrxn"));
    EXPECT_EQ(entry["oucbtrxn"], "OMVS");
  }
}

TEST(TestCBXP, ExtractFilterOnAscbOucbOucbtrxnWithExplicitIncludeOucb) {
  nlohmann::json cbdata = ExtractJSON("ascb", "oucb", "oucb.oucbtrxn=OMVS");
  ASSERT_TRUE(cbdata.is_array());
  for (const auto& entry : cbdata) {
    EXPECT_TRUE(entry.is_object());
    ASSERT_TRUE(entry.contains("ascboucb"));
    EXPECT_TRUE(entry["ascboucb"].is_object());
    ASSERT_TRUE(entry["ascboucb"].contains("oucbtrxn"));
    EXPECT_EQ(entry["ascboucb"]["oucbtrxn"], "OMVS");
  }
}

TEST(TestCBXP, ExtractFilterOnLdaxTcthwmEqual) {
  nlohmann::json cbdata = ExtractJSON("ldax", "", "ldax_tcthwm=0");
  ASSERT_TRUE(cbdata.is_array());
  for (const auto& entry : cbdata) {
    EXPECT_TRUE(entry.is_object());
    ASSERT_TRUE(entry.contains("ldax_tcthwm"));
    EXPECT_EQ(entry["ldax_tcthwm"], 0);
  }
}

TEST(TestCBXP, ExtractFilterOnPsaCvtAsvtAscbAssbLdaxTcthwmWithInclude) {
  nlohmann::json cbdata = ExtractJSON("psa", "cvt.asvt.ascb.assb.ldax",
                                      "cvt.asvt.ascb.assb.ldax.ldax_tcthwm=0");
  ASSERT_TRUE(cbdata.is_object());
  ASSERT_TRUE(cbdata.contains("flccvt"));
  EXPECT_TRUE(cbdata["flccvt"].is_object());
  ASSERT_TRUE(cbdata["flccvt"].contains("cvtasvt"));
  EXPECT_TRUE(cbdata["flccvt"]["cvtasvt"].is_object());
  ASSERT_TRUE(cbdata["flccvt"]["cvtasvt"].contains("asvtenty"));
  EXPECT_TRUE(cbdata["flccvt"]["cvtasvt"]["asvtenty"].is_array());
  EXPECT_GT(cbdata["flccvt"]["cvtasvt"]["asvtenty"].size(), 0);

  for (const auto& entry : cbdata["flccvt"]["cvtasvt"]["asvtenty"]) {
    EXPECT_TRUE(entry.is_object());
    ASSERT_TRUE(entry.contains("ascbassb"));
    EXPECT_TRUE(entry["ascbassb"].is_object());
    ASSERT_TRUE(entry["ascbassb"].contains("assbldax"));
    EXPECT_TRUE(entry["ascbassb"]["assbldax"].is_object());
    ASSERT_TRUE(entry["ascbassb"]["assbldax"].contains("ldax_tcthwm"));
    EXPECT_EQ(entry["ascbassb"]["assbldax"]["ldax_tcthwm"], 0);
  }
}

TEST(TestCBXP, ExtractNullFilterString) {
  nlohmann::json cbdata =
      ExtractJSON("assb", "", "assbjbns=*MASTER*,assbjbni=");
  EXPECT_TRUE(cbdata.is_array());
}

// ============================================================================
// Extract -- Testing Errors: Unknown Control Block
// ============================================================================

TEST(TestCBXP, ExtractRaisesErrorIfUnknownControlBlockIsProvided) {
  CBXPResultWrapper res(
      cbxp_extract("unknown", 7, nullptr, 0, nullptr, 0, false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 1);
  EXPECT_EQ(res->result_json, nullptr);
}

// ============================================================================
// Extract -- Testing Errors: Bad Include Patterns
// ============================================================================

TEST(TestCBXP, ExtractRaisesErrorIfAsvtAscbIsIncludedWithPsa) {
  CBXPResultWrapper res(
      cbxp_extract("psa", 3, "asvt.ascb", 9, nullptr, 0, false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 2);
}

TEST(TestCBXP, ExtractRaisesErrorIfAscbIsIncludedWithPsa) {
  CBXPResultWrapper res(cbxp_extract("psa", 3, "ascb", 4, nullptr, 0, false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 2);
}

TEST(TestCBXP, ExtractRaisesErrorIfEcvtIsIncludedWithAscb) {
  CBXPResultWrapper res(cbxp_extract("ascb", 4, "ecvt", 4, nullptr, 0, false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 2);
}

TEST(TestCBXP, ExtractRaisesErrorIfCvtAscbIsIncludedWithPsa) {
  CBXPResultWrapper res(
      cbxp_extract("psa", 3, "cvt.ecvt,cvt.ascb", 17, nullptr, 0, false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 2);
}

TEST(TestCBXP, ExtractRaisesErrorIfEcvtIsIncludedWithPsa) {
  CBXPResultWrapper res(
      cbxp_extract("psa", 3, "ecvt,cvt.asvt.ascb", 19, nullptr, 0, false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 2);
}

TEST(TestCBXP, ExtractRaisesErrorIfCvtIsIncludedWithCvt) {
  CBXPResultWrapper res(cbxp_extract("cvt", 3, "cvt", 3, nullptr, 0, false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 2);
}

TEST(TestCBXP, ExtractRaisesErrorIfLdaxIncludedWithPsa) {
  CBXPResultWrapper res(cbxp_extract("psa", 3, "ldax", 4, nullptr, 0, false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 2);
}

TEST(TestCBXP, ExtractRaisesErrorIfLdaxIncludedWithCvt) {
  CBXPResultWrapper res(cbxp_extract("cvt", 3, "ldax", 4, nullptr, 0, false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 2);
}

TEST(TestCBXP, ExtractRaisesErrorIfLdaxIncludedWithAscb) {
  CBXPResultWrapper res(cbxp_extract("ascb", 4, "ldax", 4, nullptr, 0, false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 2);
}

// ============================================================================
// Extract -- Testing Errors: Bad Filters
// ============================================================================

TEST(TestCBXP, ExtractRaisesErrorIfFilterUsesNonIncludedControlBlock) {
  CBXPResultWrapper res(cbxp_extract(
      "psa", 3, nullptr, 0, "cvt.asvt.ascb.assb.assbjbns=*MASTER*", 36, false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 3);
}

TEST(TestCBXP, ExtractRaisesErrorIfNonEqualityFilterUsedWithString) {
  CBXPResultWrapper res(cbxp_extract(
      "psa", 3, "**", 2, "cvt.asvt.ascb.assb.assbjbns<=*MASTER*", 37, false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 3);
}

TEST(TestCBXP, ExtractRaisesErrorIfFilterUsesUnknownKey) {
  CBXPResultWrapper res(
      cbxp_extract("psa", 3, nullptr, 0, "psapsb=PSA", 10, false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 3);
}

TEST(TestCBXP, ExtractRaisesErrorIfFilterPassesNullValueForNonString) {
  CBXPResultWrapper res(
      cbxp_extract("assb", 4, nullptr, 0, "assbasid=", 9, false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 3);
}

TEST(TestCBXP, ExtractRaisesErrorIfFilterUsesStringForNumericField) {
  CBXPResultWrapper res(
      cbxp_extract("ascb", 4, nullptr, 0, "ascbasid<=junk", 14, false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 3);
}

TEST(TestCBXP, ExtractRaisesErrorIfNoOperationProvided) {
  CBXPResultWrapper res(cbxp_extract("psa", 3, nullptr, 0, "junk", 4, false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 3);
}

// ============================================================================
// Format -- Basic Usage
// ============================================================================

TEST(TestCBXP, FormatAscb) {
  std::vector<char> data = ReadSample("ascb.bin");
  ASSERT_GT(data.size(), 0);

  CBXPResultWrapper res(
      cbxp_format("ascb", 4, data.data(), data.size(), false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 0);
  ASSERT_NE(res->result_json, nullptr);

  nlohmann::json cbdata = nlohmann::json::parse(res->result_json);
  EXPECT_TRUE(cbdata.is_object());
}

TEST(TestCBXP, FormatCvt) {
  std::vector<char> data = ReadSample("cvt.bin");
  ASSERT_GT(data.size(), 0);

  CBXPResultWrapper res(cbxp_format("cvt", 3, data.data(), data.size(), false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 0);
  ASSERT_NE(res->result_json, nullptr);

  nlohmann::json cbdata = nlohmann::json::parse(res->result_json);
  EXPECT_TRUE(cbdata.is_object());
}

TEST(TestCBXP, FormatOucb) {
  std::vector<char> data = ReadSample("oucb.bin");
  ASSERT_GT(data.size(), 0);

  CBXPResultWrapper res(
      cbxp_format("oucb", 4, data.data(), data.size(), false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 0);
  ASSERT_NE(res->result_json, nullptr);

  nlohmann::json cbdata = nlohmann::json::parse(res->result_json);
  EXPECT_TRUE(cbdata.is_object());
}

// ============================================================================
// Format -- Debug Mode
// ============================================================================

TEST(TestCBXP, FormatRunsInDebugMode) {
  std::vector<char> data = ReadSample("ascb.bin");
  ASSERT_GT(data.size(), 0);

  CBXPResultWrapper res(cbxp_format("ascb", 4, data.data(), data.size(), true));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 0);
  ASSERT_NE(res->result_json, nullptr);

  nlohmann::json cbdata = nlohmann::json::parse(res->result_json);
  EXPECT_TRUE(cbdata.is_object());
}

// ============================================================================
// Format -- Testing Errors: Unknown Control Block
// ============================================================================

TEST(TestCBXP, FormatRaisesErrorIfUnknownControlBlockIsProvided) {
  std::vector<char> data = ReadSample("ascb.bin");
  ASSERT_GT(data.size(), 0);

  CBXPResultWrapper res(
      cbxp_format("unknown", 7, data.data(), data.size(), false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 1);
  EXPECT_EQ(res->result_json, nullptr);
}

// ============================================================================
// Format -- Testing Errors: Data Too Small
// ============================================================================

TEST(TestCBXP, FormatRaisesErrorIfDataIsTooSmall) {
  // PSA requires 4096 bytes, ascb.bin is only ~584 bytes
  std::vector<char> data = ReadSample("ascb.bin");
  ASSERT_GT(data.size(), 0);

  CBXPResultWrapper res(cbxp_format("psa", 3, data.data(), data.size(), false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 4);
  EXPECT_EQ(res->result_json, nullptr);
}

// ============================================================================
// Format -- Testing Errors: Null Data Pointer
// ============================================================================

TEST(TestCBXP, FormatRaisesErrorIfDataPointerIsNull) {
  CBXPResultWrapper res(cbxp_format("ascb", 4, nullptr, 0, false));
  ASSERT_NE(res.get(), nullptr);
  EXPECT_EQ(res->return_code, 5);
  EXPECT_EQ(res->result_json, nullptr);
}
