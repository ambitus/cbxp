#ifndef __CBXP_LOGGER_H_
#define __CBXP_LOGGER_H_

#include <iostream>
#include <string>

namespace CBXP {
class Logger {
 private:
  bool debug_;
  std::string ansi_bright_red_    = "\033[91m";
  std::string ansi_bright_green_  = "\033[92m";
  std::string ansi_bright_yellow_ = "\033[93m";
  std::string ansi_reset_         = "\033[0m";
  explicit Logger();

 public:
  explicit Logger(Logger const&) = delete;
  void operator=(Logger const&)  = delete;
  static Logger& getInstance();
  void setDebug(bool debug);
  void debug(const std::string& message, const std::string& body = "") const;
  void debugAllocate(const void* p_memory, int rmode, int byte_count) const;
  void debugFree(const void* p_memory) const;
  void hexDump(const char* p_buffer, int length,
               bool intended_nullptr = false) const;
};
}  // namespace CBXP

#endif
