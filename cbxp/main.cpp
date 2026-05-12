#define _UNIX03_SOURCE

#include "cli_processing.hpp"

int main(int argc, const char* argv[]) {
  CBXP::CommandProcessor command_processor = CBXP::CommandProcessor(argc, argv);

  CBXP::CLIReturnCode returnCode           = command_processor.getReturnCode();
  if (returnCode != CBXP::CLIReturnCode::NONE) {
    return returnCode;
  }

  return command_processor.process();
}
