#include <algorithm>
#include <cstring>
#include <iostream>
#include <nlohmann/json.hpp>

#include "cbxp_result.h"
#include "control_block_error.hpp"
#include "control_block_explorer.hpp"

static void show_usage(const char* argv[]) {
  std::cout << "Usage: " << argv[0] << " [options] <control block>" << std::endl
            << std::endl;

  std::cout << "Options:" << std::endl
            << "  -d, --debug                      Write debug messages"
            << std::endl
            << "  -i, --include <pattern>          Include additional control "
               "blocks based on a pattern"
            << std::endl
            << "  -v, --version                    Show version number"
            << std::endl
            << "  -h, --help                       Show usage information"
            << std::endl
            << std::endl;
}

int main(int argc, const char* argv[]) {
  bool debug                     = false;
  std::string control_block_name = "", includes_string = "";

  if (argc < 2) {
    show_usage(argv);
    return -1;
  }

  if (argc == 2) {
    if (std::strcmp(argv[1], "-v") == 0 ||
        std::strcmp(argv[1], "--version") == 0) {
      std::cout << "CBXP " << VERSION << std::endl;
      return 0;
    }

    if (std::strcmp(argv[1], "-h") == 0 ||
        std::strcmp(argv[1], "--help") == 0) {
      show_usage(argv);
      return 0;
    }
  }

  for (int i = 1; i < argc; i++) {
    std::string flag = argv[i];
    if (flag == "-d" || flag == "--debug") {
      debug = true;
    } else if (flag == "-i" || flag == "--include") {
      if (i + 1 >= argc - 1) {
        show_usage(argv);
        return -1;
      }
      std::string include = std::string(argv[++i]);
      bool has_comma      = std::any_of(include.begin(), include.end(),
                                        [](char c) { return c == ','; });
      if (has_comma) {
        std::cerr << "Include patterns cannot contain commas" << std::endl;
        return -1;
      }
      if (includes_string == "") {
        includes_string = include;
      } else {
        includes_string += "," + include;
      }
    } else {
      if (i != argc - 1) {
        show_usage(argv);
        return -1;
      }
      control_block_name = std::string(argv[i]);
    }
  }

  if (control_block_name == "") {
    show_usage(argv);
    return -1;
  }

  nlohmann::json control_block_json;

  static cbxp_result_t cbxp_result = {nullptr, 0, -1};

  CBXP::ControlBlockExplorer explorer =
      CBXP::ControlBlockExplorer(&cbxp_result, debug);

  explorer.exploreControlBlock(control_block_name, includes_string);

  if (cbxp_result.return_code == CBXP::Error::BadControlBlock) {
    std::cerr << "Unknown control block '" << control_block_name
              << "' was specified." << std::endl;
    return -1;
  } else if (cbxp_result.return_code == CBXP::Error::BadInclude) {
    std::cerr << "A bad include pattern was provided" << std::endl;
    return -1;
  } else {
    std::cout << cbxp_result.result_json << std::endl;
  }

  return 0;
}
