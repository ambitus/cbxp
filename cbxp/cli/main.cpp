#include "command_processor.hpp"

int main(int argc, const char* argv[]) {
  try {
    CBXP::CommandProcessor command_processor =
        CBXP::CommandProcessor(argc, argv);
    command_processor.run();
  } catch (const CBXP::CLIExitFailure& e) {
    return -1;
  } catch (const CBXP::CLIExitSuccess& e) {
    return 0;
  }

  return 0;
}
