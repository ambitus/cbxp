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
#include "cli_processing.hpp"
#include "control_block_error.hpp"

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
