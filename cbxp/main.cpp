#define _UNIX03_SOURCE

#include <dlfcn.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <nlohmann/json.hpp>

#include "cbxp_result.h"
#include "control_block_error.hpp"

typedef const cbxp_result_t* (*cbxp_t)(const char* control_block_name,
                                       const char* includes_string,
                                       const char* filters_string, bool debug);

typedef const void (*cbxp_free_t)(const cbxp_result_t* cbxp_result, bool debug);

static void show_usage(const char* argv[]);
static void show_dll_errors();
static void cleanup_and_exit(int exit_rc, void* lib_handle);

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

static void show_dll_errors() {
  const char* error_string = dlerror();
  if (error_string != nullptr) {
    std::cerr << error_string << std::endl;
  }
}

static void cleanup_and_exit(int exit_rc, void* lib_handle) {
  int rc = dlclose(lib_handle);
  if (rc != 0) {
    show_dll_errors();
    exit(-1);
  }
  exit(exit_rc);
}

bool check_for_comma(const std::string& string) {
  return std::any_of(string.begin(), string.end(),
                     [](char c) { return c == ','; });
}

int main(int argc, const char* argv[]) {
  // Load 'libcbxp.so'
  void* lib_handle = dlopen("libcbxp.so", RTLD_NOW);
  if (lib_handle == nullptr) {
    show_dll_errors();
    return -1;
  }

  int exit_rc = -1;
  // Resolve symbol 'cbxp()'
  cbxp_t cbxp = reinterpret_cast<cbxp_t>(dlsym(lib_handle, "cbxp"));
  if (cbxp == nullptr) {
    show_dll_errors();
    cleanup_and_exit(exit_rc, lib_handle);
  }
  cbxp_free_t cbxp_free =
      reinterpret_cast<cbxp_free_t>(dlsym(lib_handle, "cbxp_free"));
  if (cbxp_free == nullptr) {
    show_dll_errors();
    cleanup_and_exit(exit_rc, lib_handle);
  }

  bool debug                     = false;
  std::string control_block_name = "", includes_string = "",
              filters_string = "";

  if (argc < 2) {
    show_usage(argv);
    cleanup_and_exit(exit_rc, lib_handle);
  }

  if (argc == 2) {
    if (std::strcmp(argv[1], "-v") == 0 ||
        std::strcmp(argv[1], "--version") == 0) {
      std::cout << "CBXP " << VERSION << std::endl;
      cleanup_and_exit(0, lib_handle);
    }

    if (std::strcmp(argv[1], "-h") == 0 ||
        std::strcmp(argv[1], "--help") == 0) {
      show_usage(argv);
      cleanup_and_exit(0, lib_handle);
    }
  }

  for (int i = 1; i < argc; i++) {
    std::string flag = argv[i];
    if (flag == "-d" || flag == "--debug") {
      if (!debug) {
        debug = true;
      } else {
        show_usage(argv);
        cleanup_and_exit(exit_rc, lib_handle);
      }
    } else if (flag == "-i" || flag == "--include") {
      if (i + 1 >= argc - 1) {
        show_usage(argv);
        cleanup_and_exit(exit_rc, lib_handle);
      }
      std::string include = std::string(argv[++i]);
      if (check_for_comma(include)) {
        std::cerr << "Include patterns cannot contain commas" << std::endl;
        cleanup_and_exit(exit_rc, lib_handle);
      }
      if (includes_string == "") {
        includes_string = include;
      } else {
        includes_string += "," + include;
      }
    } else if (flag == "-f" || flag == "--filter") {
      if (i + 1 >= argc - 1) {
        show_usage(argv);
        cleanup_and_exit(exit_rc, lib_handle);
      }
      std::string filter = std::string(argv[++i]);
      if (check_for_comma(filter)) {
        std::cerr << "Filters cannot contain commas" << std::endl;
        cleanup_and_exit(exit_rc, lib_handle);
      }
      if (filters_string == "") {
        filters_string = filter;
      } else {
        filters_string += "," + filter;
      }
    } else {
      if (i != argc - 1) {
        show_usage(argv);
        cleanup_and_exit(exit_rc, lib_handle);
      }
      control_block_name = std::string(argv[i]);
    }
  }

  if (control_block_name == "") {
    show_usage(argv);
    cleanup_and_exit(exit_rc, lib_handle);
  }

  nlohmann::json control_block_json;

  const cbxp_result_t* cbxp_result =
      cbxp(control_block_name.c_str(), includes_string.c_str(),
           filters_string.c_str(), debug);

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
      exit_rc = 0;
  }

  cbxp_free(cbxp_result, debug);
  cleanup_and_exit(exit_rc, lib_handle);
}
