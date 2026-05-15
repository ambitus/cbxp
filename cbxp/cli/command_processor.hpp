#ifndef __CBXP_CLI_H_
#define __CBXP_CLI_H_

#include <algorithm>
#include <cstring>
#include <string>
#include <vector>

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
  std::vector<char> data_buffer;
  unsigned int offset;
} cbxp_format_options_t;

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
  void readFormatDataFromPipe();
  void readFormatDataFromFile(const std::string& file_path);
  static bool isGlobalFlag(const std::string& flag);
  static bool checkForComma(const std::string& string);

 public:
  explicit CommandProcessor(int argc, const char* argv[])
      : argc_(argc),
        argv_(argv),
        global_options_({false, false, false}),
        extract_options_({"", ""}),
        format_options_({"", {}, 0}) {
    process();
  };
  void run();
};
}  // namespace CBXP

#endif
