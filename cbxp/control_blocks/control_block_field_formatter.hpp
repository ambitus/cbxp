#ifndef __CB_EXPLORER_CONTROL_BLOCK_FIELD_FORMATTER
#define __CB_EXPLORER_CONTROL_BLOCK_FIELD_FORMATTER

#include <unistd.h>

#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>

namespace CBXP {
class ControlBlockFieldFormatter {
 public:
  template <typename T>
  static T uint(const void* p_field) {
    T uint_field;
    std::memcpy(reinterpret_cast<char*>(&uint_field), p_field, sizeof(T));
    return uint_field;
  }

  static const std::string getString(const void* p_field, int length) {
    std::vector<char> ascii_field_tmp(length, 0);
    std::memcpy(ascii_field_tmp.data(), p_field, length);
    __e2a_l(ascii_field_tmp.data(), length);
    std::string ascii_field(ascii_field_tmp.begin(), ascii_field_tmp.end());
    size_t last_non_space = ascii_field.find_last_not_of(" \t\n\r\f\v");
    ascii_field.resize(last_non_space + 1);
    last_non_space = ascii_field.find_last_not_of('\0');
    ascii_field.resize(last_non_space + 1);
    return ascii_field;
  }
  template <typename T>
  static const std::string getHex(const void* p_field) {
    std::ostringstream oss;
    oss << "0x" << std::hex << std::setfill('0');
    oss << std::setw(sizeof(T) * 2)
        << ControlBlockFieldFormatter::uint<T>(p_field);
    return oss.str();
  }
  template <typename T>
  static const std::string getBitmap(const void* p_field) {
    std::ostringstream oss;
    oss << std::bitset<sizeof(T) * 8>{
        ControlBlockFieldFormatter::uint<T>(p_field)};
    return oss.str();
  }
  template <typename T>
  static const std::string getBitmap(T field) {
    std::ostringstream oss;
    oss << std::bitset<sizeof(T) * 8>{field};
    return oss.str();
  }
  static const std::string getPswSmall(const unsigned char* p_field) {
    std::ostringstream oss;
    oss << getBitmap<uint32_t>(p_field);
    oss << " | ";
    oss << getHex<uint32_t>(p_field + 4);
    return oss.str();
  }
  // Do we plan on using this???
  // cppcheck-suppress unusedFunction
  static const std::string getPswBig(const unsigned char* p_field) {
    std::ostringstream oss;
    oss << getBitmap<uint64_t>(p_field);
    oss << " | ";
    oss << getHex<uint64_t>(p_field + 8);
    return oss.str();
  }
};
}  // namespace CBXP

#endif
