#ifndef __CBXP_CLI_H_
#define __CBXP_CLI_H_

#include <algorithm>
#include <cstring>
#include <string>

namespace CBXP {

enum CLIReturnCode { SUCCESS = 0, FAILURE = -1, NONE = 1 };

typedef struct {
  bool debug;
  bool version;
  bool help;
  int debug_ind;
} cbxp_global_options_t;

typedef struct {
  std::string include;
  std::string filter;
} cbxp_extract_options_t;

typedef struct {
  std::string file;
  char* data_buffer;
  int buffer_length;
  int offset;
} cbxp_format_options_t;

static bool checkForComma(const std::string& string);

class CommandProcessor {
 private:
  std::string command_;
  std::string control_block_name_;
  int argc_;
  const char** argv_;
  CLIReturnCode return_code_;
  cbxp_global_options_t global_options_;
  cbxp_extract_options_t extract_options_;
  cbxp_format_options_t format_options_;
  void showGeneralUsage() const;
  void showExtractUsage() const;
  void showFormatUsage() const;
  void parse();
  CLIReturnCode parseGlobalFlags();
  CLIReturnCode parseFormatFlags();
  CLIReturnCode parseExtractFlags();
  void processFormatFlags();

 public:
  explicit CommandProcessor(int argc, const char* argv[])
      : argc_(argc),
        argv_(argv),
        return_code_(CLIReturnCode::NONE),
        global_options_({false, false, false, 0}),
        extract_options_({"", ""}),
        format_options_({"", nullptr, 0, 0}) {
    parse();
  };
  CLIReturnCode getReturnCode() const;
  CLIReturnCode process();
};
}  // namespace CBXP

#endif
