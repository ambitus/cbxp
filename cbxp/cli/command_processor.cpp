#include "command_processor.hpp"

#include <unistd.h>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "cbxp.h"
#include "control_block_error.hpp"

namespace CBXP {

void CommandProcessor::showGeneralUsage() const {
  std::cout << "Full CLI documentation is available at: "
               "https://ambitus.github.io/cbxp/interfaces/shell/"
            << std::endl
            << std::endl;

  std::cout << "Usage:" << std::endl
            << "  " << argv_[0] << " [command]" << std::endl
            << std::endl;

  std::cout << "Available Commands:" << std::endl
            << "extract                            Extract and format control "
               "block data from live memory"
            << std::endl
            << "format                             Format control block data "
               "from a file/pipe"
            << std::endl
            << std::endl;

  std::cout << "Flags:" << std::endl
            << "  -d, --debug                      Write debug messages"
            << std::endl
            << "  -v, --version                    Show version number"
            << std::endl
            << "  -h, --help                       Show usage information"
            << std::endl
            << std::endl
            << "Use \"" << argv_[0]
            << " [command] --help\" for more information about a command."
            << std::endl;
}

void CommandProcessor::showExtractUsage() const {
  std::cout << "Extract and format control block data from live memory."
            << std::endl
            << std::endl;

  std::cout << "Usage:" << std::endl
            << "  " << argv_[0] << " extract [flags] <control block>"
            << std::endl
            << std::endl;

  std::cout << "Examples: " << std::endl
            << "  # Extract the PSA control block from live memory and write "
               "debug messages."
            << std::endl
            << "  " << argv_[0] << " extract -d psa" << std::endl
            << std::endl
            << "  # Extract the CVT control block from live memory, including "
               "the ECVT, ASVT,"
            << std::endl
            << "  # and all known control blocks that are pointed to directly "
               "by the ASVT "
            << std::endl
            << "  # control block." << std::endl
            << "  " << argv_[0] << " extract -i ecvt -i 'asvt.*' cvt"
            << std::endl
            << std::endl
            << "  # Extract all ASSB control blocks from live memory where the "
               "control block."
            << std::endl
            << "  # field 'ASSBJBNI' matches the filter value 'IBMUSER'."
            << std::endl
            << "  " << argv_[0] << " extract -f assb.assbjbni=IBMUSER assb"
            << std::endl
            << std::endl;

  std::cout << "Flags:" << std::endl
            << "  -i, --include <pattern>          Include additional control "
               "blocks based on a pattern"
            << std::endl
            << "  -f, --filter <filter>            Filter repeated control "
               "block data"
            << std::endl
            << std::endl;

  std::cout << "Global Flags:" << std::endl
            << "  -d, --debug                      Write debug messages"
            << std::endl
            << "  -h, --help                       Show usage information"
            << std::endl;
}

void CommandProcessor::showFormatUsage() const {
  std::cout << "Format control block data from a file/pipe." << std::endl
            << std::endl;

  std::cout << "Usage:" << std::endl
            << "  " << argv_[0] << " format [flags] <control block>"
            << std::endl
            << std::endl;

  std::cout << "Examples: " << std::endl
            << "  # Format CVT control block data from a file and write "
               "debug messages."
            << std::endl
            << "  " << argv_[0] << " format -F /path/to/cvt.bin -d cvt"
            << std::endl
            << std::endl
            << "  # Format CVT control block data from a pipe." << std::endl
            << "  cat /path/to/cvt.bin | " << argv_[0] << " format cvt"
            << std::endl
            << std::endl
            << "  # Format OUCB control block data from a file at an offset "
               "of 0x03A8 bytes."
            << std::endl
            << "  " << argv_[0]
            << " format -F /path/to/oucboffset.bin -o x3A8 oucb" << std::endl
            << std::endl
            << "  # Format ASCB control block data from a data set at an "
               "offset of 64 bytes."
            << std::endl
            << "  " << argv_[0] << " format -F \"//'HLQ.ASCBOF64'\" -o 64 ascb"
            << std::endl
            << std::endl;

  std::cout << "Flags:" << std::endl
            << "  -F, --file  <path>               Format control block data "
               "from a specified file or dataset"
            << std::endl
            << "  -o, --offset <value>             Specify an offset into "
               "a memory buffer to start formatting"
            << std::endl
            << std::endl;

  std::cout << "Global Flags:" << std::endl
            << "  -d, --debug                      Write debug messages"
            << std::endl
            << "  -h, --help                       Show usage information"
            << std::endl;
}

void CommandProcessor::process() {
  CommandProcessor::processGlobalFlags();

  if (command_ == "format") {
    if (global_options_.help) {
      CommandProcessor::showFormatUsage();
      throw CLIExitSuccess();
    } else {
      CommandProcessor::processFormatFlags();
    }
  } else if (command_ == "extract") {
    if (global_options_.help) {
      CommandProcessor::showExtractUsage();
      throw CLIExitSuccess();
    } else {
      CommandProcessor::processExtractFlags();
    }
  }
}

void CommandProcessor::processGlobalFlags() {
  if (argc_ < 2) {
    CommandProcessor::showGeneralUsage();
    throw CLIExitFailure();
  }

  if (std::strcmp(argv_[1], "-v") == 0 ||
      std::strcmp(argv_[1], "--version") == 0) {
    global_options_.version = true;
    std::cout << "CBXP " << VERSION << std::endl;
    throw CLIExitSuccess();
  } else if (std::strcmp(argv_[1], "-h") == 0 ||
             std::strcmp(argv_[1], "--help") == 0) {
    global_options_.help = true;
    CommandProcessor::showGeneralUsage();
    throw CLIExitSuccess();
  }

  command_ = argv_[1];

  if (command_ == "format" || command_ == "extract") {
    if (argc_ < 3) {
      std::cerr << ERROR_CONTROL_BLOCK_EXPECTED_ << std::endl;
      throw CLIExitFailure();
    }
  } else if (command_[0] != '-') {
    std::cerr << ERROR_UNKNOWN_COMMMAND_ << command_ << std::endl;
    throw CLIExitFailure();
  } else if (!CommandProcessor::isGlobalFlag(command_)) {
    std::cerr << ERROR_UNKNOWN_FLAG_ << command_ << std::endl;
    throw CLIExitFailure();
  } else {
    CommandProcessor::showGeneralUsage();
    throw CLIExitFailure();
  }

  int argv_index = 2;  // argv 1 is the command, 2 is the first flag

  if (std::strcmp(argv_[2], "-h") == 0 ||
      std::strcmp(argv_[2], "--help") == 0) {
    argv_index++;
    global_options_.help = true;
  }

  for (int i = argv_index; i < argc_; i++) {
    std::string flag = argv_[i];
    if (flag == "-d" || flag == "--debug") {
      global_options_.debug = true;
    }
  }

  control_block_name_ = std::string(argv_[argc_ - 1]);

  if (control_block_name_ == "" || control_block_name_[0] == '-') {
    std::cerr << ERROR_CONTROL_BLOCK_EXPECTED_ << std::endl;
    throw CLIExitFailure();
  }
}

void CommandProcessor::processExtractFlags() {
  for (int i = 2; i < argc_; i++) {
    std::string flag = argv_[i];
    if (flag == "-i" || flag == "--include") {
      if (i + 1 >= argc_ - 1 || argv_[i + 1][0] == '-') {
        std::cerr << ERROR_FLAG_NEEDS_AN_ARGUMENT_ << flag << std::endl;
        throw CLIExitFailure();
      }
      std::string include = std::string(argv_[++i]);
      if (CommandProcessor::checkForComma(include)) {
        std::cerr << ERROR_INCLUDES_CANT_HAVE_COMMAS_ << std::endl;
        throw CLIExitFailure();
      }
      if (extract_options_.include == "") {
        extract_options_.include = include;
      } else {
        extract_options_.include += "," + include;
      }
    } else if (flag == "-f" || flag == "--filter") {
      if (i + 1 >= argc_ - 1 || argv_[i + 1][0] == '-') {
        std::cerr << ERROR_FLAG_NEEDS_AN_ARGUMENT_ << flag << std::endl;
        throw CLIExitFailure();
      }
      std::string filter = std::string(argv_[++i]);
      if (CommandProcessor::checkForComma(filter)) {
        std::cerr << ERROR_FILTERS_CANT_HAVE_COMMAS_ << std::endl;
        throw CLIExitFailure();
      }
      if (extract_options_.filter == "") {
        extract_options_.filter = filter;
      } else {
        extract_options_.filter += "," + filter;
      }
    } else if (i != argc_ - 1 || flag[0] == '-') {
      if (CommandProcessor::isGlobalFlag(flag)) {
        continue;
      }
      std::cerr << ERROR_UNKNOWN_FLAG_ << flag << std::endl;
      throw CLIExitFailure();
    }
  }
}

void CommandProcessor::processFormatFlags() {
  format_options_.file = "";

  for (int i = 2; i < argc_; i++) {
    std::string flag = argv_[i];
    if (flag == "-F" || flag == "--file") {
      if (i + 1 >= argc_ - 1 || argv_[i + 1][0] == '-') {
        std::cerr << ERROR_FLAG_NEEDS_AN_ARGUMENT_ << flag << std::endl;
        throw CLIExitFailure();
      }
      CommandProcessor::readFormatDataFromFile(std::string(argv_[++i]));
    } else if (flag == "-o" || flag == "--offset") {
      if (i + 1 >= argc_ - 1 || argv_[i + 1][0] == '-') {
        std::cerr << ERROR_FLAG_NEEDS_AN_ARGUMENT_ << flag << std::endl;
        throw CLIExitFailure();
      }
      std::string offset = argv_[++i];
      if (offset.find('.') != std::string::npos) {
        std::cerr << ERROR_OFFSET_MUST_BE_A_POSITIVE_INTEGER_ << std::endl;
        throw CLIExitFailure();
      }
      try {
        format_options_.offset = std::stoul(offset, nullptr, 0);
      }
      // Standard exceptions for stoi
      catch (const std::invalid_argument& e) {
        // Offset not positive or not integer
        std::cerr << ERROR_OFFSET_MUST_BE_A_POSITIVE_INTEGER_ << std::endl;
        throw CLIExitFailure();
      } catch (const std::out_of_range& e) {
        // Offset too large
        std::cerr << ERROR_OFFSET_TOO_LARGE_ << std::endl;
        throw CLIExitFailure();
      }
    } else if (i != argc_ - 1 || flag[0] == '-') {
      if (CommandProcessor::isGlobalFlag(flag)) {
        continue;
      }
      std::cerr << ERROR_UNKNOWN_FLAG_ << flag << std::endl;
      throw CLIExitFailure();
    }
  }
  if (isatty(STDIN_FILENO) == 0) {
    CommandProcessor::readFormatDataFromPipe();
  }
  if (format_options_.data_buffer.empty()) {
    std::cerr << ERROR_FILE_OR_PIPE_EXPECTED_ << std::endl;
    throw CLIExitFailure();
  }

  if (format_options_.offset > format_options_.data_buffer.size()) {
    std::cerr << ERROR_OFFSET_TOO_LARGE_ << std::endl;
    throw CLIExitFailure();
  }
}

void CommandProcessor::readFormatDataFromPipe() {
  if (!format_options_.file.empty()) {
    std::cerr << ERROR_FILE_CANT_HAVE_STDIN_ << std::endl;
    throw CLIExitFailure();
  }
  format_options_.data_buffer.assign((std::istreambuf_iterator<char>(std::cin)),
                                     (std::istreambuf_iterator<char>()));
  if (!format_options_.data_buffer.empty()) {
    std::string env_p(std::getenv("_BPXK_AUTOCVT"));
    if (!env_p.empty() && (env_p == "ON" || env_p == "ALL")) {
      __a2e_l(format_options_.data_buffer.data(),
              format_options_.data_buffer.size());
    }
  }
}

void CommandProcessor::readFormatDataFromFile(const std::string& file_path) {
  format_options_.file = file_path;
  std::ifstream file(format_options_.file, std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    std::cerr << "Error opening file: " << format_options_.file << std::endl;
    throw CLIExitFailure();
  }
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);  // Move back to start

  format_options_.data_buffer.resize(size);
  file.read(format_options_.data_buffer.data(), size);
}

bool CommandProcessor::isGlobalFlag(const std::string& flag) {
  if (flag == "-d" || flag == "--debug") {
    return true;
  }
  if (flag == "-h" || flag == "--help") {
    return true;
  }
  if (flag == "-v" || flag == "--version") {
    return true;
  }
  return false;
}

bool CommandProcessor::checkForComma(const std::string& string) {
  return std::any_of(string.begin(), string.end(),
                     [](char c) { return c == ','; });
}

void CommandProcessor::run() {
  cbxp_result_t* cbxp_result;

  if (command_ == "extract") {
    cbxp_result = cbxp_extract(
        control_block_name_.c_str(), control_block_name_.length(),
        extract_options_.include.c_str(), extract_options_.include.length(),
        extract_options_.filter.c_str(), extract_options_.filter.length(),
        global_options_.debug);
  } else {
    cbxp_result =
        cbxp_format(control_block_name_.c_str(), control_block_name_.length(),
                    static_cast<void*>(format_options_.data_buffer.data() +
                                       format_options_.offset),
                    format_options_.data_buffer.size() - format_options_.offset,
                    global_options_.debug);
  }

  switch (cbxp_result->return_code) {
    case CBXP::Error::BadControlBlock:
      std::cerr << ERROR_UNKNOWN_CONTROL_BLOCK_ << control_block_name_
                << std::endl;
      throw CLIExitFailure();
      break;
    case CBXP::Error::BadInclude:
      std::cerr << ERROR_BAD_INCLUDE_ << std::endl;
      throw CLIExitFailure();
      break;
    case CBXP::Error::BadFilter:
      std::cerr << ERROR_BAD_FILTER_ << std::endl;
      throw CLIExitFailure();
      break;
    case CBXP::Error::BufferTooSmall:
      std::cerr << ERROR_BUFFER_TOO_SMALL_ << control_block_name_ << std::endl;
      throw CLIExitFailure();
      break;
    default:
      std::cout << cbxp_result->result_json << std::endl;
  }

  cbxp_free(cbxp_result, global_options_.debug);
}

}  // namespace CBXP
