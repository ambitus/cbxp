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
  bool help;
} cbxp_global_options_t;

typedef struct {
  std::string include;
  std::string filter;
} cbxp_extract_options_t;

typedef struct {
  std::string file;
  std::vector<char> data_buffer;
  size_t offset;
} cbxp_format_options_t;

class CommandProcessor {
 private:
  // Error Strings
  // General
  const std::string ERROR_FLAG_NEEDS_AN_ARGUMENT_ = "Flag needs an argument: ";
  const std::string ERROR_UNKNOWN_COMMMAND_       = "Unknown command: ";
  const std::string ERROR_UNKNOWN_FLAG_           = "Unknown flag: ";
  const std::string ERROR_CONTROL_BLOCK_EXPECTED_ =
      "Positional argument <control block> expected";
  // Format
  const std::string ERROR_OFFSET_MUST_BE_A_POSITIVE_INTEGER_ =
      "Offset must be a positive integer";
  const std::string ERROR_OFFSET_TOO_LARGE_ =
      "Offset is too large for data provided";
  const std::string ERROR_OPENING_FILE_ = "Error opening file: ";
  const std::string ERROR_FILE_OR_PIPE_EXPECTED_ =
      "File or pipe expected for \"format\" command";
  const std::string ERROR_FILE_AND_PIPE_CANT_BE_USED_TOGETHER_ =
      "File and pipe cannot be used together";
  // Extract
  const std::string ERROR_COMMA_IN_INCLUDE_ =
      "Include patterns cannot contain commas";
  const std::string ERROR_COMMA_IN_FILTER_ =
      "Include patterns cannot contain commas";
  // Return Codes
  const std::string ERROR_UNKNOWN_CONTROL_BLOCK_ = "Unknown control block: ";
  const std::string ERROR_BAD_INCLUDE_ = "A bad include pattern was provided";
  const std::string ERROR_BAD_FILTER_  = "A bad filter was provided";
  const std::string ERROR_DATA_TOO_SMALL_ =
      "Data provided is not large enough for specified control block: ";

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
        global_options_({false, false}),
        extract_options_({"", ""}),
        format_options_({"", {}, 0}) {
    CommandProcessor::process();
  };
  void run();
};
}  // namespace CBXP

#endif
