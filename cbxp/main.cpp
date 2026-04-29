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

static void show_usage(const char* argv[]);

enum CLIReturnCode { SUCCESS = 0, FAILURE = -1 };

static void show_usage(const char* argv[]) {
  std::cout << "Usage: " << argv[0] << "[operation] [options] <control block>"
            << std::endl
            << std::endl;

  std::cout << "Operations:" << std::endl
            << "explore                            Explore control blocks in "
               "system memory"
            << std::endl
            << "format                             Format control blocks from "
               "a buffer/file"
            << std::endl
            << "Note: format requires either -F/--file option or piping binary "
               "data as input"
            << std::endl
            << std::endl;

  std::cout
      << "Options:" << std::endl
      << "  -d, --debug                      Write debug messages" << std::endl
      << "  -i, --include <pattern>          Include additional control "
         "blocks based on a pattern (explore only)"
      << std::endl
      << "  -f, --filter <filter>            Filter repeated control "
         "block data (explore only)"
      << std::endl
      << "  -F, --file  <path>               Format control block data "
         "from a specified file or dataset (format only)"
      << std::endl
      << "  -o, --offset <value>             Specify an offset into "
         "a memory buffer to start formatting (format only)"
      << std::endl
      << "  -v, --version                    Show version number (no operation)"
      << std::endl
      << "  -h, --help                       Show usage information (no "
         "operation)"
      << std::endl
      << std::endl;
}

bool check_for_comma(const std::string& string) {
  return std::any_of(string.begin(), string.end(),
                     [](char c) { return c == ','; });
}

int main(int argc, const char* argv[]) {
  bool debug                     = false;
  std::string control_block_name = "", includes_string = "",
              filters_string = "";
  uint64_t offset = 0, buffer_length = -1;
  bool offset_specified = false, file_specified = false, data_piped = false;
  std::vector<char> input_buffer;
  char* data_buffer = nullptr;

  if (argc == 2) {
    if (std::strcmp(argv[1], "-v") == 0 ||
        std::strcmp(argv[1], "--version") == 0) {
      std::cout << "CBXP " << VERSION << std::endl;
      return CLIReturnCode::SUCCESS;
    }

    if (std::strcmp(argv[1], "-h") == 0 ||
        std::strcmp(argv[1], "--help") == 0) {
      show_usage(argv);
      return CLIReturnCode::SUCCESS;
    }
  }

  if (argc < 3) {
    show_usage(argv);
    return CLIReturnCode::FAILURE;
  }

  std::string operation = argv[1];
  if (operation != "explore" && operation != "format") {
    std::cerr << "cbxp must perform 'format' or 'explore' operation"
              << std::endl;
    show_usage(argv);
    return CLIReturnCode::FAILURE;
  }

  if (isatty(STDIN_FILENO) == 0) {
    input_buffer.assign((std::istreambuf_iterator<char>(std::cin)),
                        (std::istreambuf_iterator<char>()));
    if (!input_buffer.empty()) {
      data_buffer   = input_buffer.data();
      buffer_length = input_buffer.size();
    }
    data_piped = true;
  }

  for (int i = 2; i < argc; i++) {
    std::string flag = argv[i];
    if (flag == "-d" || flag == "--debug") {
      if (!debug) {
        debug = true;
      } else {
        show_usage(argv);
        return CLIReturnCode::FAILURE;
      }
    } else if (flag == "-i" || flag == "--include") {
      if (i + 1 >= argc - 1) {
        show_usage(argv);
        return CLIReturnCode::FAILURE;
      }
      std::string include = std::string(argv[++i]);
      if (check_for_comma(include)) {
        std::cerr << "Include patterns cannot contain commas" << std::endl;
        return CLIReturnCode::FAILURE;
      }
      if (includes_string == "") {
        includes_string = include;
      } else {
        includes_string += "," + include;
      }
    } else if (flag == "-f" || flag == "--filter") {
      if (i + 1 >= argc - 1) {
        show_usage(argv);
        return CLIReturnCode::FAILURE;
      }
      std::string filter = std::string(argv[++i]);
      if (check_for_comma(filter)) {
        std::cerr << "Filters cannot contain commas" << std::endl;
        return CLIReturnCode::FAILURE;
      }
      if (filters_string == "") {
        filters_string = filter;
      } else {
        filters_string += "," + filter;
      }
    } else if (flag == "-F" || flag == "--file") {
      if (i + 1 >= argc - 1) {
        show_usage(argv);
        return CLIReturnCode::FAILURE;
      }
      file_specified       = true;
      std::string filename = std::string(argv[++i]);
      std::streamsize size;
      if (filename.rfind("//", 0) == 0) {
        std::ifstream dsFile(filename, std::ios::binary | std::ios::ate);
        if (!dsFile.is_open()) {
          std::cerr << "Error opening dataset: " << filename << std::endl;
          show_usage(argv);
          return CLIReturnCode::FAILURE;
        }
        size = dsFile.tellg();
        dsFile.seekg(0, std::ios::beg);  // Move back to start

        input_buffer.resize(size);
        if (dsFile.read(input_buffer.data(), size)) {
          data_buffer = input_buffer.data();
        }
      } else {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
          std::cerr << "Error opening file: " << filename << std::endl;
          show_usage(argv);
          return CLIReturnCode::FAILURE;
        }
        size = file.tellg();
        file.seekg(0, std::ios::beg);  // Move back to start

        input_buffer.resize(size);
        if (file.read(input_buffer.data(), size)) {
          data_buffer = input_buffer.data();
        }
      }
      buffer_length = size;

    } else if (flag == "-o" || flag == "--offset") {
      if (i + 1 >= argc - 1) {
        show_usage(argv);
        return CLIReturnCode::FAILURE;
      }
      offset_specified = true;
      offset           = std::stoull(argv[++i], nullptr, 0);
    } else {
      if (i != argc - 1) {
        show_usage(argv);
        return CLIReturnCode::FAILURE;
      }
      control_block_name = std::string(argv[i]);
    }
  }

  if (control_block_name == "") {
    show_usage(argv);
    return CLIReturnCode::FAILURE;
  }

  if (data_piped && file_specified) {
    std::cerr << "File parameter cannot be used with STDIN data" << std::endl;
    show_usage(argv);
    return CLIReturnCode::FAILURE;
  }

  nlohmann::json control_block_json;
  cbxp_result_t* cbxp_result;

  if (operation == "explore") {
    if (offset_specified || data_buffer != nullptr) {
      std::cerr
          << "Files and Offsets cannot be used with the 'explore' operation"
          << std::endl;
      show_usage(argv);
      return CLIReturnCode::FAILURE;
    }
    cbxp_result =
        cbxp_extract(control_block_name.c_str(), includes_string.c_str(),
                     filters_string.c_str(), debug);
  }

  if (operation == "format") {
    if (filters_string != "" || includes_string != "") {
      std::cerr
          << "Filters and Includes cannot be used with the 'format' operation"
          << std::endl;
      show_usage(argv);
      return CLIReturnCode::FAILURE;
    }
    if (data_buffer == nullptr) {
      std::cerr
          << "File parameter or STDIN data required for 'format' operation"
          << std::endl;
      show_usage(argv);
      return CLIReturnCode::FAILURE;
    }
    if (offset >= buffer_length) {
      std::cerr << "Offset is too large for specified data/file" << std::endl;
      show_usage(argv);
      return CLIReturnCode::FAILURE;
    }
    cbxp_result = cbxp_format(control_block_name.c_str(),
                              static_cast<void*>(data_buffer + offset),
                              buffer_length - offset, debug);
  }

  CLIReturnCode cli_return_code = CLIReturnCode::FAILURE;

  switch (cbxp_result->return_code) {
    case CBXP::Error::BadControlBlock:
      std::cerr << "Unknown control block '" << control_block_name
                << "' was specified." << std::endl;
      break;
    case CBXP::Error::BadInclude:
      std::cerr << "A bad include pattern was provided" << std::endl;
      break;
    case CBXP::Error::BadFilter:
      std::cerr << "A bad filter was provided" << std::endl;
      break;
    default:
      std::cout << cbxp_result->result_json << std::endl;
      cli_return_code = CLIReturnCode::SUCCESS;
  }

  cbxp_free(cbxp_result, debug);

  return cli_return_code;
}
