#ifndef __CB_EXPLORER_CONTROL_BLOCK_FIELD_FORMATTER
#define __CB_EXPLORER_CONTROL_BLOCK_FIELD_FORMATTER

#include <unistd.h>

#include <bitset>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "../control_block_error.hpp"
#include "../safe_read.hpp"

namespace CBXP {
class ControlBlockFieldFormatter {
 public:
  // Every read of live storage goes through readOrThrow(), so an unreadable
  // address (e.g. fetch-protected) raises CbxpBlockUnreachableError instead of
  // abending with S0C4.
  template <typename T>
  static T uint(const void* p_field) {
    T uint_field;
    readOrThrow(&uint_field, p_field, sizeof(T));
    return uint_field;
  }

  template <typename T>
  static T sint(const void* p_field) {
    T sint_field;
    readOrThrow(&sint_field, p_field, sizeof(T));
    return sint_field;
  }

  static const std::string getString(const void* p_field, int length) {
    std::vector<char> ascii_field_tmp(length, 0);
    readOrThrow(ascii_field_tmp.data(), p_field, length);
    __e2a_l(ascii_field_tmp.data(), length);
    std::string ascii_field(ascii_field_tmp.begin(), ascii_field_tmp.end());
    size_t last_non_space =
        ascii_field.find_last_not_of({'\0', ' ', '\t', '\n', '\r', '\f', '\v'});
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

 private:
  static void readOrThrow(void* p_dest, const void* p_src, size_t length) {
    if (!safeCopy(p_dest, p_src, length)) {
      throw CbxpBlockUnreachableError();
    }
  }
};
}  // namespace CBXP

#endif
