#include "cli_processing.hpp"

#include <dlfcn.h>
#include <unistd.h>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "cbxp.h"
#include "control_block_error.hpp"

namespace CBXP {

bool checkForComma(const std::string& string) {
  return std::any_of(string.begin(), string.end(),
                     [](char c) { return c == ','; });
}

void CommandProcessor::showGeneralUsage() const {
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

  std::cout
      << "Flags:" << std::endl
      << "  -d, --debug                      Write debug messages" << std::endl
      << "  -v, --version                    Show version number" << std::endl
      << "  -h, --help                       Show usage information"
      << std::endl
      << std::endl
      << "Use \"cbxp [command] --help\" for more information about a command."
      << std::endl
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
            << "  cbxp extract -d psa" << std::endl
            << "  # Extract the CVT control block from live memory, including "
               "the ECVT, ASVT,"
            << std::endl
            << "  # and all known control blocks that are pointed to directly "
               "by the ASVT "
            << std::endl
            << " # control block." << std::endl
            << "  cbxp extract -i ecvt -i 'asvt.*' cvt" << std::endl
            << "  # Extract all ASSB control blocks from live memory where the "
               "control block."
            << std::endl
            << "   # field 'ASSBJBNI' matches the filter value 'IBMUSER'."
            << std::endl
            << "  cbxp extract -f assb.assbjbni=IBMUSER assb" << std::endl
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
            << std::endl
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
            << "  cbxp format -F tests/samples/cvt.bin -d cvt" << std::endl
            << "  # Format CVT control block data from a pipe." << std::endl
            << "  cat tests/samples/cvt.bin | cbxp format cvt" << std::endl
            << "  # Format OUCB control block data from a file at an offset "
               "of 0x03A8 bytes."
            << std::endl
            << "  cbxp format -F tests/samples/oucboffset3A8.bin -o x3A8 oucb"
            << std::endl
            << "  # Format ASCB control block data from a data set at an "
               "offset of 64 bytes."
            << std::endl
            << "  cbxp format -F \"//'CBXPUSR.ASCBOF64'\" -o 64 ascb"
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
            << std::endl
            << std::endl;
}

void CommandProcessor::parse() {
  return_code_ = CommandProcessor::parseGlobalFlags();

  if (return_code_ != CLIReturnCode::NONE) {
    return;
  }

  if (command_ == "format") {
    if (global_options_.help) {
      showFormatUsage();
      return_code_ = CLIReturnCode::SUCCESS;
    } else {
      return_code_ = CommandProcessor::parseFormatFlags();
    }
  } else if (command_ == "extract") {
    if (global_options_.help) {
      CommandProcessor::showExtractUsage();
      return_code_ = CLIReturnCode::SUCCESS;
    } else {
      return_code_ = CommandProcessor::parseExtractFlags();
    }
  } else {
    std::cerr << "unknown command \"" << command_ << "\"for " << argv_[0]
              << std::endl;
    CommandProcessor::showGeneralUsage();
    return_code_ = CLIReturnCode::FAILURE;
  }
}

CLIReturnCode CommandProcessor::parseGlobalFlags() {
  if (argc_ == 2) {
    if (std::strcmp(argv_[1], "-v") == 0 ||
        std::strcmp(argv_[1], "--version") == 0) {
      global_options_.version = true;
      std::cout << "CBXP " << VERSION << std::endl;
      return CLIReturnCode::SUCCESS;
    }
    if (std::strcmp(argv_[1], "-h") == 0 ||
        std::strcmp(argv_[1], "--help") == 0) {
      global_options_.help = true;
      CommandProcessor::showGeneralUsage();
      return CLIReturnCode::SUCCESS;
    }
  }
  if (argc_ < 3) {
    CommandProcessor::showGeneralUsage();
    return CLIReturnCode::FAILURE;
  }
  command_       = argv_[1];

  int argv_index = 2;  // argv 1 is the command, 2 is the first flag

  if (std::strcmp(argv_[2], "-h") == 0 ||
      std::strcmp(argv_[2], "--help") == 0) {
    argv_index++;
    global_options_.help = true;
  }

  for (int i = argv_index; i < argc_; i++) {
    std::string flag = argv_[i];
    if (flag == "-d" || flag == "--debug") {
      if (!global_options_.debug) {
        global_options_.debug_ind = i;
        global_options_.debug     = true;
      } else {
        CommandProcessor::showGeneralUsage();
        return CLIReturnCode::FAILURE;
      }
    }
  }

  control_block_name_ = std::string(argv_[argc_ - 1]);
  return CLIReturnCode::NONE;
}

CLIReturnCode CommandProcessor::parseExtractFlags() {
  for (int i = 2; i < argc_; i++) {
    std::string flag = argv_[i];
    if (flag == "-i" || flag == "--include") {
      if (i + 1 >= argc_ - 1) {
        CommandProcessor::showExtractUsage();
        return CLIReturnCode::FAILURE;
      }
      std::string include = std::string(argv_[++i]);
      if (checkForComma(include)) {
        std::cerr << "Include patterns cannot contain commas" << std::endl;
        return CLIReturnCode::FAILURE;
      }
      if (extract_options_.include == "") {
        extract_options_.include = include;
      } else {
        extract_options_.include += "," + include;
      }
    } else if (flag == "-f" || flag == "--filter") {
      if (i + 1 >= argc_ - 1) {
        CommandProcessor::showExtractUsage();
        return CLIReturnCode::FAILURE;
      }
      std::string filter = std::string(argv_[++i]);
      if (checkForComma(filter)) {
        std::cerr << "Filters cannot contain commas" << std::endl;
        return CLIReturnCode::FAILURE;
      }
      if (extract_options_.filter == "") {
        extract_options_.filter = filter;
      } else {
        extract_options_.filter += "," + filter;
      }
    } else {
      if ((i != argc_ - 1) && (i != global_options_.debug_ind)) {
        CommandProcessor::showExtractUsage();
        return CLIReturnCode::FAILURE;
      }
    }
  }

  return CLIReturnCode::NONE;
}

CLIReturnCode CommandProcessor::parseFormatFlags() {
  format_options_.file = "";

  for (int i = 2; i < argc_; i++) {
    std::string flag = argv_[i];
    if (flag == "-F" || flag == "--file") {
      if (i + 1 >= argc_ - 1) {
        CommandProcessor::showFormatUsage();
        return CLIReturnCode::FAILURE;
      }
      format_options_.file = std::string(argv_[++i]);

    } else if (flag == "-o" || flag == "--offset") {
      if (i + 1 >= argc_ - 1) {
        CommandProcessor::showFormatUsage();
        return CLIReturnCode::FAILURE;
      }
      format_options_.offset = std::stoi(argv_[++i], nullptr, 0);
    } else {
      if ((i != argc_ - 1) && (i != global_options_.debug_ind)) {
        CommandProcessor::showFormatUsage();
        return CLIReturnCode::FAILURE;
      }
    }
  }

  return CLIReturnCode::NONE;
}

void CommandProcessor::processFormatFlags() {
  format_options_.data_buffer   = nullptr;
  format_options_.buffer_length = -1;
  std::vector<char> input_buffer;

  if (format_options_.offset < 0) {
    std::cerr << "Offset parameter can not be negative" << std::endl;
    CommandProcessor::showFormatUsage();
    return_code_ = CLIReturnCode::FAILURE;
    return;
  }

  if (isatty(STDIN_FILENO) == 0) {
    if (!format_options_.file.empty()) {
      std::cerr << "File parameter cannot be used with STDIN data" << std::endl;
      CommandProcessor::showFormatUsage();
      return_code_ = CLIReturnCode::FAILURE;
      return;
    }
    input_buffer.assign((std::istreambuf_iterator<char>(std::cin)),
                        (std::istreambuf_iterator<char>()));
    if (!input_buffer.empty()) {
      format_options_.data_buffer   = input_buffer.data();
      format_options_.buffer_length = input_buffer.size();
      std::string env_p(std::getenv("_BPXK_AUTOCVT"));
      if (!env_p.empty() && (env_p == "ON" || env_p == "ALL")) {
        __a2e_l(format_options_.data_buffer, format_options_.buffer_length);
      }
    }
  }
  if (!format_options_.file.empty()) {
    std::ifstream dsFile(format_options_.file,
                         std::ios::binary | std::ios::ate);
    if (!dsFile.is_open()) {
      std::cerr << "Error opening input: " << format_options_.file << std::endl;
      CommandProcessor::showFormatUsage();
      return_code_ = CLIReturnCode::FAILURE;
      return;
    }
    std::streamsize size = dsFile.tellg();
    dsFile.seekg(0, std::ios::beg);  // Move back to start

    input_buffer.resize(size);
    if (dsFile.read(input_buffer.data(), size)) {
      format_options_.data_buffer = input_buffer.data();
    }
    format_options_.buffer_length = size;
  }

  if (format_options_.data_buffer == nullptr) {
    std::cerr << "File parameter or STDIN data required for 'format' operation"
              << std::endl;
    CommandProcessor::showFormatUsage();
    return_code_ = CLIReturnCode::FAILURE;
    return;
  }

  if (format_options_.buffer_length < 0) {
    std::cerr << "Error opening input: " << format_options_.file << std::endl;
    CommandProcessor::showFormatUsage();
    return_code_ = CLIReturnCode::FAILURE;
    return;
  }

  if (format_options_.offset >= format_options_.buffer_length) {
    std::cerr << "Offset is too large for data provided" << std::endl;
    CommandProcessor::showFormatUsage();
    return_code_ = CLIReturnCode::FAILURE;
    return;
  }
}

CLIReturnCode CommandProcessor::getReturnCode() const { return return_code_; }

CLIReturnCode CommandProcessor::process() {
  if (control_block_name_ == "") {
    CommandProcessor::showGeneralUsage();
    return CLIReturnCode::FAILURE;
  }

  if (command_ == "format") {
    CommandProcessor::processFormatFlags();
    if (return_code_ != CLIReturnCode::NONE) {
      return return_code_;
    }
  }

  cbxp_result_t* cbxp_result;

  if (command_ == "extract") {
    cbxp_result = cbxp_extract(
        control_block_name_.c_str(), extract_options_.include.c_str(),
        extract_options_.filter.c_str(), global_options_.debug);
  } else {
    cbxp_result =
        cbxp_format(control_block_name_.c_str(),
                    static_cast<void*>(format_options_.data_buffer +
                                       format_options_.offset),
                    format_options_.buffer_length - format_options_.offset,
                    global_options_.debug);
  }
  return_code_ = CBXP::CLIReturnCode::FAILURE;

  switch (cbxp_result->return_code) {
    case CBXP::Error::BadControlBlock:
      std::cerr << "Unknown control block '" << control_block_name_
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
                << control_block_name_ << "' control block" << std::endl;
      break;
    default:
      std::cout << cbxp_result->result_json << std::endl;
      return_code_ = CBXP::CLIReturnCode::SUCCESS;
  }

  cbxp_free(cbxp_result, global_options_.debug);

  return return_code_;
}

}  // namespace CBXP
