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
                                       const char* includes_string, bool debug);

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

int main(int argc, const char* argv[]) {
  // Load 'libcbxp.so'
  void* lib_handle = dlopen("libcbxp.so", RTLD_NOW);
  if (lib_handle == nullptr) {
    show_dll_errors();
    return -1;
  }

  // Resolve symbol 'cbxp()'
  cbxp_t cbxp = reinterpret_cast<cbxp_t>(dlsym(lib_handle, "cbxp"));
  if (cbxp == nullptr) {
    show_dll_errors();
    cleanup_and_exit(-1, lib_handle);
  }

  bool debug                     = false;
  std::string control_block_name = "", includes_string = "";

  if (argc < 2) {
    show_usage(argv);
    cleanup_and_exit(-1, lib_handle);
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
      debug = true;
    } else if (flag == "-i" || flag == "--include") {
      if (i + 1 >= argc - 1) {
        show_usage(argv);
        cleanup_and_exit(-1, lib_handle);
      }
      std::string include = std::string(argv[++i]);
      bool has_comma      = std::any_of(include.begin(), include.end(),
                                        [](char c) { return c == ','; });
      if (has_comma) {
        std::cerr << "Include patterns cannot contain commas" << std::endl;
        cleanup_and_exit(-1, lib_handle);
      }
      if (includes_string == "") {
        includes_string = include;
      } else {
        includes_string += "," + include;
      }
    } else {
      if (i != argc - 1) {
        show_usage(argv);
        cleanup_and_exit(-1, lib_handle);
      }
      control_block_name = std::string(argv[i]);
    }
  }

  if (control_block_name == "") {
    show_usage(argv);
    cleanup_and_exit(-1, lib_handle);
  }

  nlohmann::json control_block_json;

  const cbxp_result_t* cbxp_result =
      cbxp(control_block_name.c_str(), includes_string.c_str(), debug);

  if (cbxp_result->return_code == CBXP::Error::BadControlBlock) {
    std::cerr << "Unknown control block '" << control_block_name
              << "' was specified." << std::endl;
    cleanup_and_exit(-1, lib_handle);
  } else if (cbxp_result->return_code == CBXP::Error::BadInclude) {
    std::cerr << "A bad include pattern was provided" << std::endl;
    cleanup_and_exit(-1, lib_handle);
  } else {
    std::cout << cbxp_result->result_json << std::endl;
  }

  cleanup_and_exit(0, lib_handle);
}
