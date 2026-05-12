#define _UNIX03_SOURCE

#include <dlfcn.h>
#include <unistd.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>

#include "cbxp.h"
#include "control_block_error.hpp"

static void showGeneralUsage(const char* argv[]);

enum CLIReturnCode { SUCCESS = 0, FAILURE = -1, NONE = 1 };

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

static void showGeneralUsage(const char* argv[]) {
  std::cout << "Usage: " << argv[0] << "[command] [options] <control block>"
            << std::endl
            << std::endl;

  std::cout << "Available Commands:" << std::endl
            << "extract                            Extract control blocks in "
               "system memory"
            << std::endl
            << "format                             Format control blocks from "
               "a buffer/file"
            << std::endl
            << std::endl;

  std::cout
      << "Options:" << std::endl
      << "  -d, --debug                      Write debug messages" << std::endl
      << "  -v, --version                    Show version number (no operation)"
      << std::endl
      << "  -h, --help                       Show usage information (no "
         "operation)"
      << std::endl
      << std::endl;
}

static void showExtractUsage(const char* argv[]) {
  std::cout << "Usage: " << argv[0] << "extract [options] <control block>"
            << std::endl
            << std::endl;

  std::cout << "Options:" << std::endl
            << "  -d, --debug                      Write debug messages"
            << std::endl
            << "  -i, --include <pattern>          Include additional control "
               "blocks based on a pattern"
            << std::endl
            << "  -f, --filter <filter>            Filter repeated control "
               "block data"
            << std::endl
            << "  -h, --help                       Show usage information (no "
               "operation)"
            << std::endl
            << std::endl;
}

static void showFormatUsage(const char* argv[]) {
  std::cout << "Usage: " << argv[0] << "format [options] <control block>"
            << std::endl
            << std::endl;

  std::cout << "Options:" << std::endl
            << "  -d, --debug                      Write debug messages"
            << std::endl
            << "  -F, --file  <path>               Format control block data "
               "from a specified file or dataset"
            << std::endl
            << "  -o, --offset <value>             Specify an offset into "
               "a memory buffer to start formatting"
            << std::endl
            << "  -h, --help                       Show usage information (no "
               "operation)"
            << std::endl
            << std::endl;
}

bool checkForComma(const std::string& string) {
  return std::any_of(string.begin(), string.end(),
                     [](char c) { return c == ','; });
}

int parseFormatOptions(int argc, const char* argv[],
                       cbxp_command_t* cbxp_command) {
  cbxp_command->format_options.file = "";

  for (int i = 2; i < argc; i++) {
    std::string flag = argv[i];
    if (flag == "-d" || flag == "--debug") {
      if (!cbxp_command->debug) {
        cbxp_command->debug = true;
      } else {
        showGeneralUsage(argv);
        return CLIReturnCode::FAILURE;
      }
    } else if (flag == "-F" || flag == "--file") {
      if (i + 1 >= argc - 1) {
        showFormatUsage(argv);
        return CLIReturnCode::FAILURE;
      }
      cbxp_command->format_options.file = std::string(argv[++i]);

    } else if (flag == "-o" || flag == "--offset") {
      if (i + 1 >= argc - 1) {
        showFormatUsage(argv);
        return CLIReturnCode::FAILURE;
      }
      cbxp_command->format_options.offset = std::stoull(argv[++i], nullptr, 0);
    } else {
      if (i != argc - 1) {
        showFormatUsage(argv);
        return CLIReturnCode::FAILURE;
      }
      cbxp_command->control_block_name = std::string(argv[i]);
    }
  }

  return CLIReturnCode::NONE;
}

int parseExtractOptions(int argc, const char* argv[],
                        cbxp_command_t* cbxp_command) {
  for (int i = 2; i < argc; i++) {
    std::string flag = argv[i];
    if (flag == "-d" || flag == "--debug") {
      if (cbxp_command->debug) {
        cbxp_command->debug = true;
      } else {
        showGeneralUsage(argv);
        return CLIReturnCode::FAILURE;
      }
    } else if (flag == "-i" || flag == "--include") {
      if (i + 1 >= argc - 1) {
        showExtractUsage(argv);
        return CLIReturnCode::FAILURE;
      }
      std::string include = std::string(argv[++i]);
      if (checkForComma(include)) {
        std::cerr << "Include patterns cannot contain commas" << std::endl;
        return CLIReturnCode::FAILURE;
      }
      if (cbxp_command->extract_options.include == "") {
        cbxp_command->extract_options.include = include;
      } else {
        cbxp_command->extract_options.include += "," + include;
      }
    } else if (flag == "-f" || flag == "--filter") {
      if (i + 1 >= argc - 1) {
        showExtractUsage(argv);
        return CLIReturnCode::FAILURE;
      }
      std::string filter = std::string(argv[++i]);
      if (checkForComma(filter)) {
        std::cerr << "Filters cannot contain commas" << std::endl;
        return CLIReturnCode::FAILURE;
      }
      if (cbxp_command->extract_options.filter == "") {
        cbxp_command->extract_options.filter = filter;
      } else {
        cbxp_command->extract_options.filter += "," + filter;
      }
    } else {
      if (i != argc - 1) {
        showExtractUsage(argv);
        return CLIReturnCode::FAILURE;
      }
      cbxp_command->control_block_name = std::string(argv[i]);
    }
  }

  return CLIReturnCode::NONE;
}

int parseOptions(int argc, const char* argv[], cbxp_command_t* cbxp_command) {
  if (argc == 2) {
    if (std::strcmp(argv[1], "-v") == 0 ||
        std::strcmp(argv[1], "--version") == 0) {
      std::cout << "CBXP " << VERSION << std::endl;
      return CLIReturnCode::SUCCESS;
    }
    if (std::strcmp(argv[1], "-h") == 0 ||
        std::strcmp(argv[1], "--help") == 0) {
      showGeneralUsage(argv);
      return CLIReturnCode::SUCCESS;
    }
  }
  if (argc < 3) {
    showGeneralUsage(argv);
    return CLIReturnCode::FAILURE;
  }
  cbxp_command->command = argv[1];
  cbxp_command->debug   = false;

  if (cbxp_command->command == "format") {
    if (std::strcmp(argv[2], "-h") == 0 ||
        std::strcmp(argv[2], "--help") == 0) {
      showFormatUsage(argv);
      return CLIReturnCode::SUCCESS;
    }
    return parseFormatOptions(argc, argv, cbxp_command);
  } else if (cbxp_command->command == "extract") {
    if (std::strcmp(argv[2], "-h") == 0 ||
        std::strcmp(argv[2], "--help") == 0) {
      showExtractUsage(argv);
      return CLIReturnCode::SUCCESS;
    }
    return parseExtractOptions(argc, argv, cbxp_command);
  } else {
    std::cerr << "cbxp must perform 'format' or 'extract' operation"
              << std::endl;
    showGeneralUsage(argv);
    return CLIReturnCode::FAILURE;
  }

  return CLIReturnCode::NONE;
}

int processFormatOptions(const char* argv[], cbxp_command_t* cbxp_command) {
  cbxp_command->format_options.data_buffer   = nullptr;
  cbxp_command->format_options.buffer_length = -1;
  std::vector<char> input_buffer;

  if (isatty(STDIN_FILENO) == 0) {
    if (!cbxp_command->format_options.file.empty()) {
      std::cerr << "File parameter cannot be used with STDIN data" << std::endl;
      showGeneralUsage(argv);
      return CLIReturnCode::FAILURE;
    }
    input_buffer.assign((std::istreambuf_iterator<char>(std::cin)),
                        (std::istreambuf_iterator<char>()));
    if (!input_buffer.empty()) {
      cbxp_command->format_options.data_buffer   = input_buffer.data();
      cbxp_command->format_options.buffer_length = input_buffer.size();
      std::string env_p(std::getenv("_BPXK_AUTOCVT"));
      if (!env_p.empty() && (env_p == "ON" || env_p == "ALL")) {
        __a2e_l(cbxp_command->format_options.data_buffer,
                cbxp_command->format_options.buffer_length);
      }
    }
  }
  if (!cbxp_command->format_options.file.empty()) {
    std::ifstream dsFile(cbxp_command->format_options.file,
                         std::ios::binary | std::ios::ate);
    if (!dsFile.is_open()) {
      std::cerr << "Error opening input: " << cbxp_command->format_options.file
                << std::endl;
      showFormatUsage(argv);
      return CLIReturnCode::FAILURE;
    }
    std::streamsize size = dsFile.tellg();
    dsFile.seekg(0, std::ios::beg);  // Move back to start

    input_buffer.resize(size);
    if (dsFile.read(input_buffer.data(), size)) {
      cbxp_command->format_options.data_buffer = input_buffer.data();
    }
    cbxp_command->format_options.buffer_length = size;
  }

  if (cbxp_command->format_options.data_buffer == nullptr) {
    std::cerr << "File parameter or STDIN data required for 'format' operation"
              << std::endl;
    showFormatUsage(argv);
    return CLIReturnCode::FAILURE;
  }

  if (cbxp_command->format_options.offset >=
      cbxp_command->format_options.buffer_length) {
    std::cerr << "Offset is too large for data provided" << std::endl;
    showFormatUsage(argv);
    return CLIReturnCode::FAILURE;
  }

  return CLIReturnCode::NONE;
}

int processOptions(const char* argv[], cbxp_command_t* cbxp_command) {
  if (cbxp_command->control_block_name == "") {
    showGeneralUsage(argv);
    return CLIReturnCode::FAILURE;
  }

  if (cbxp_command->command == "format") {
    return processFormatOptions(argv, cbxp_command);
  }

  return CLIReturnCode::NONE;
}

int callCbxp(const cbxp_command_t& cbxp_command) {
  cbxp_result_t* cbxp_result;

  if (cbxp_command.command == "extract") {
    cbxp_result = cbxp_extract(cbxp_command.control_block_name.c_str(),
                               cbxp_command.extract_options.include.c_str(),
                               cbxp_command.extract_options.filter.c_str(),
                               cbxp_command.debug);
  } else {
    cbxp_result =
        cbxp_format(cbxp_command.control_block_name.c_str(),
                    static_cast<void*>(cbxp_command.format_options.data_buffer +
                                       cbxp_command.format_options.offset),
                    cbxp_command.format_options.buffer_length -
                        cbxp_command.format_options.offset,
                    cbxp_command.debug);
  }
  CLIReturnCode cli_return_code = CLIReturnCode::FAILURE;

  switch (cbxp_result->return_code) {
    case CBXP::Error::BadControlBlock:
      std::cerr << "Unknown control block '" << cbxp_command.control_block_name
                << "' was specified." << std::endl;
      break;
    case CBXP::Error::BadInclude:
      std::cerr << "A bad include pattern was provided" << std::endl;
      break;
    case CBXP::Error::BadFilter:
      std::cerr << "A bad filter was provided" << std::endl;
      break;
    case CBXP::Error::BufferTooSmall:
      std::cerr << "The buffer is not large enough to contain a '"
                << cbxp_command.control_block_name << "'" << std::endl;
      break;
    default:
      std::cout << cbxp_result->result_json << std::endl;
      cli_return_code = CLIReturnCode::SUCCESS;
  }

  cbxp_free(cbxp_result, cbxp_command.debug);

  return cli_return_code;
}

int main(int argc, const char* argv[]) {
  int returnCode = CLIReturnCode::NONE;
  cbxp_command_t cbxp_command;

  returnCode = parseOptions(argc, argv, &cbxp_command);
  if (returnCode != CLIReturnCode::NONE) {
    return returnCode;
  }

  returnCode = processOptions(argv, &cbxp_command);
  if (returnCode != CLIReturnCode::NONE) {
    return returnCode;
  }

  return callCbxp(cbxp_command);
}
