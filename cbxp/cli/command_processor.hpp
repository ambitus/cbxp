#ifndef __CBXP_CLI_H_
#define __CBXP_CLI_H_

#include <algorithm>
#include <cstring>
#include <string>

namespace CBXP {

class CLIExitSuccess : public std::exception {};

class CLIExitFailure : public std::exception {};

typedef struct {
  bool debug;
  bool version;
  bool help;
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
  cbxp_global_options_t global_options_;
  cbxp_extract_options_t extract_options_;
  cbxp_format_options_t format_options_;
  void showGeneralUsage() const;
  void showExtractUsage() const;
  void showFormatUsage() const;
  void process();
  void processGlobalFlags();
  void processFormatFlags();
  void processExtractFlags();
  void processFormatFromPipe();
  void processFormatFromFile(const std::string& file_path);

 public:
  explicit CommandProcessor(int argc, const char* argv[])
      : argc_(argc),
        argv_(argv),
        global_options_({false, false, false}),
        extract_options_({"", ""}),
        format_options_({"", nullptr, 0, 0}) {
    process();
  };
  void run();
};
}  // namespace CBXP

#endif
