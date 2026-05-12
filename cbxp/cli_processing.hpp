#ifndef __CBXP_CLI_H_
#define __CBXP_CLI_H_

#include <algorithm>
#include <cstring>
#include <string>

static void showGeneralUsage(const char* argv[]);
static void showExtractUsage(const char* argv[]);
static void showFormatUsage(const char* argv[]);

static bool checkForComma(const std::string& string);

enum CLIReturnCode { SUCCESS = 0, FAILURE = -1, NONE = 1 };

int parseOptions(int argc, const char* argv[], cbxp_command_t* cbxp_command);
int parseFormatOptions(int argc, const char* argv[],
                       cbxp_command_t* cbxp_command);
int parseExtractOptions(int argc, const char* argv[],
                        cbxp_command_t* cbxp_command);

int processOptions(const char* argv[], cbxp_command_t* cbxp_command);
int processFormatOptions(const char* argv[], cbxp_command_t* cbxp_command);

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

typedef struct {
  std::string command;
  std::string control_block_name;
  bool debug;
  cbxp_extract_options_t extract_options;
  cbxp_format_options_t format_options;
} cbxp_command_t;

#endif
