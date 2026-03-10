#define _UNIX03_SOURCE

#include <dlfcn.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <nlohmann/json.hpp>

#include "cbxp.h"
#include "control_block_error.hpp"

static void show_usage(const char* argv[]);

enum CLIReturnCode { SUCCESS = 0, FAILURE = -1 };

static void show_usage(const char* argv[]) {
  std::cout << "Usage: " << argv[0] << " [options] <control block>" << std::endl
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
            << "  -v, --version                    Show version number"
            << std::endl
            << "  -h, --help                       Show usage information"
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

  if (argc < 2) {
    show_usage(argv);
    return CLIReturnCode::FAILURE;
  }

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

  for (int i = 1; i < argc; i++) {
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

  nlohmann::json control_block_json;

  cbxp_result_t* cbxp_result =
      cbxp(control_block_name.c_str(), includes_string.c_str(),
           filters_string.c_str(), debug);

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
