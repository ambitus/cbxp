#include "cli_processing.hpp"

#include <dlfcn.h>
#include <unistd.h>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

static void showGeneralUsage(const char* argv[]) {
  std::cout << "Usage: " << argv[0] << " [command]" << std::endl << std::endl;

  std::cout << "Available Commands:" << std::endl
            << "extract                            Extract control block data "
               "from live memory"
            << std::endl
            << "format                             Format control block data "
               "from file/pipe"
            << std::endl
            << std::endl;

  std::cout
      << "Flags:" << std::endl
      << "  -v, --version                    Show version number" << std::endl
      << "  -h, --help                       Show usage information"
      << std::endl
      << std::endl
      << "Use \"cbxp [command] --help\" for more information about a command."
      << std::endl
      << std::endl;
}

static void showExtractUsage(const char* argv[]) {
  std::cout << "Extract and format control block data from live memory"
            << std::endl
            << std::endl;

  std::cout << "Usage: " << argv[0] << " extract [flags] <control block>"
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
               "control block"
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

static void showFormatUsage(const char* argv[]) {
  std::cout << "Format control block data from file/pipe" << std::endl
            << std::endl;

  std::cout << "Usage: " << argv[0] << " format [flags] <control block>"
            << std::endl
            << std::endl;

  std::cout << "Examples: " << std::endl
            << "  # Format the CVT control block from a binary file and write "
               "debug messages."
            << std::endl
            << "  cbxp format -F tests/samples/cvt.bin -d cvt" << std::endl
            << "  # Format the CVT control block from a binary data piped to "
               "cbxp through stdin"
            << std::endl
            << "  cat tests/samples/cvt.bin | cbxp format cvt" << std::endl
            << "  # Format the OUCB control block from a binary file at offset "
               "'x3A8' bytes"
            << std::endl
            << "  cbxp format -F tests/samples/oucboffset3A8.bin -o x3A8 oucb"
            << std::endl
            << "  # Format the ASCB control block from a dataset at offset "
               "64 bytes"
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

bool checkForComma(const std::string& string) {
  return std::any_of(string.begin(), string.end(),
                     [](char c) { return c == ','; });
}

int parseFlags(int argc, const char* argv[], cbxp_command_t* cbxp_command) {
  if (argc == 2) {
    if (std::strcmp(argv[1], "-v") == 0 ||
        std::strcmp(argv[1], "--version") == 0) {
      std::cout << "CBXP " << VERSION << std::endl;
      return CLIReturnCode::SUCCESS;
    }
    if (std::strcmp(argv[1], "-h") == 0 ||
        std::strcmp(argv[1], "--help") == 0) {
      showGeneralUsage(argv);
      return CLIReturnCode::SUCCESS;
    }
  }
  if (argc < 3) {
    showGeneralUsage(argv);
    return CLIReturnCode::FAILURE;
  }
  cbxp_command->command = argv[1];
  cbxp_command->debug   = false;

  if (cbxp_command->command == "format") {
    if (std::strcmp(argv[2], "-h") == 0 ||
        std::strcmp(argv[2], "--help") == 0) {
      showFormatUsage(argv);
      return CLIReturnCode::SUCCESS;
    }
    return parseFormatFlags(argc, argv, cbxp_command);
  } else if (cbxp_command->command == "extract") {
    if (std::strcmp(argv[2], "-h") == 0 ||
        std::strcmp(argv[2], "--help") == 0) {
      showExtractUsage(argv);
      return CLIReturnCode::SUCCESS;
    }
    return parseExtractFlags(argc, argv, cbxp_command);
  } else {
    std::cerr << "unknown command \"" << cbxp_command->command << "\"for cbxp"
              << std::endl;
    showGeneralUsage(argv);
    return CLIReturnCode::FAILURE;
  }

  return CLIReturnCode::NONE;
}

int parseExtractFlags(int argc, const char* argv[],
                      cbxp_command_t* cbxp_command) {
  for (int i = 2; i < argc; i++) {
    std::string flag = argv[i];
    if (flag == "-d" || flag == "--debug") {
      if (!cbxp_command->debug) {
        cbxp_command->debug = true;
      } else {
        showGeneralUsage(argv);
        return CLIReturnCode::FAILURE;
      }
    } else if (flag == "-i" || flag == "--include") {
      if (i + 1 >= argc - 1) {
        showExtractUsage(argv);
        return CLIReturnCode::FAILURE;
      }
      std::string include = std::string(argv[++i]);
      if (checkForComma(include)) {
        std::cerr << "Include patterns cannot contain commas" << std::endl;
        return CLIReturnCode::FAILURE;
      }
      if (cbxp_command->extract_options.include == "") {
        cbxp_command->extract_options.include = include;
      } else {
        cbxp_command->extract_options.include += "," + include;
      }
    } else if (flag == "-f" || flag == "--filter") {
      if (i + 1 >= argc - 1) {
        showExtractUsage(argv);
        return CLIReturnCode::FAILURE;
      }
      std::string filter = std::string(argv[++i]);
      if (checkForComma(filter)) {
        std::cerr << "Filters cannot contain commas" << std::endl;
        return CLIReturnCode::FAILURE;
      }
      if (cbxp_command->extract_options.filter == "") {
        cbxp_command->extract_options.filter = filter;
      } else {
        cbxp_command->extract_options.filter += "," + filter;
      }
    } else {
      if (i != argc - 1) {
        showExtractUsage(argv);
        return CLIReturnCode::FAILURE;
      }
      cbxp_command->control_block_name = std::string(argv[i]);
    }
  }

  return CLIReturnCode::NONE;
}

int parseFormatFlags(int argc, const char* argv[],
                     cbxp_command_t* cbxp_command) {
  cbxp_command->format_options.file = "";

  for (int i = 2; i < argc; i++) {
    std::string flag = argv[i];
    if (flag == "-d" || flag == "--debug") {
      if (!cbxp_command->debug) {
        cbxp_command->debug = true;
      } else {
        showGeneralUsage(argv);
        return CLIReturnCode::FAILURE;
      }
    } else if (flag == "-F" || flag == "--file") {
      if (i + 1 >= argc - 1) {
        showFormatUsage(argv);
        return CLIReturnCode::FAILURE;
      }
      cbxp_command->format_options.file = std::string(argv[++i]);

    } else if (flag == "-o" || flag == "--offset") {
      if (i + 1 >= argc - 1) {
        showFormatUsage(argv);
        return CLIReturnCode::FAILURE;
      }
      cbxp_command->format_options.offset = std::stoi(argv[++i], nullptr, 0);
    } else {
      if (i != argc - 1) {
        showFormatUsage(argv);
        return CLIReturnCode::FAILURE;
      }
      cbxp_command->control_block_name = std::string(argv[i]);
    }
  }

  return CLIReturnCode::NONE;
}

int processFlags(const char* argv[], cbxp_command_t* cbxp_command) {
  if (cbxp_command->control_block_name == "") {
    showGeneralUsage(argv);
    return CLIReturnCode::FAILURE;
  }

  if (cbxp_command->command == "format") {
    return processFormatFlags(argv, cbxp_command);
  }

  return CLIReturnCode::NONE;
}

int processFormatFlags(const char* argv[], cbxp_command_t* cbxp_command) {
  cbxp_command->format_options.data_buffer   = nullptr;
  cbxp_command->format_options.buffer_length = -1;
  std::vector<char> input_buffer;

  if (cbxp_command->format_options.offset < 0) {
    std::cerr << "Offset parameter can not be negative" << std::endl;
    showFormatUsage(argv);
    return CLIReturnCode::FAILURE;
  }

  if (isatty(STDIN_FILENO) == 0) {
    if (!cbxp_command->format_options.file.empty()) {
      std::cerr << "File parameter cannot be used with STDIN data" << std::endl;
      showGeneralUsage(argv);
      return CLIReturnCode::FAILURE;
    }
    input_buffer.assign((std::istreambuf_iterator<char>(std::cin)),
                        (std::istreambuf_iterator<char>()));
    if (!input_buffer.empty()) {
      cbxp_command->format_options.data_buffer   = input_buffer.data();
      cbxp_command->format_options.buffer_length = input_buffer.size();
      std::string env_p(std::getenv("_BPXK_AUTOCVT"));
      if (!env_p.empty() && (env_p == "ON" || env_p == "ALL")) {
        __a2e_l(cbxp_command->format_options.data_buffer,
                cbxp_command->format_options.buffer_length);
      }
    }
  }
  if (!cbxp_command->format_options.file.empty()) {
    std::ifstream dsFile(cbxp_command->format_options.file,
                         std::ios::binary | std::ios::ate);
    if (!dsFile.is_open()) {
      std::cerr << "Error opening input: " << cbxp_command->format_options.file
                << std::endl;
      showFormatUsage(argv);
      return CLIReturnCode::FAILURE;
    }
    std::streamsize size = dsFile.tellg();
    dsFile.seekg(0, std::ios::beg);  // Move back to start

    input_buffer.resize(size);
    if (dsFile.read(input_buffer.data(), size)) {
      cbxp_command->format_options.data_buffer = input_buffer.data();
    }
    cbxp_command->format_options.buffer_length = size;
  }

  if (cbxp_command->format_options.data_buffer == nullptr) {
    std::cerr << "File parameter or STDIN data required for 'format' operation"
              << std::endl;
    showFormatUsage(argv);
    return CLIReturnCode::FAILURE;
  }

  if (cbxp_command->format_options.buffer_length < 0) {
    std::cerr << "Error opening input: " << cbxp_command->format_options.file
              << std::endl;
    showFormatUsage(argv);
    return CLIReturnCode::FAILURE;
  }

  if (cbxp_command->format_options.offset >=
      cbxp_command->format_options.buffer_length) {
    std::cerr << "Offset is too large for data provided" << std::endl;
    showFormatUsage(argv);
    return CLIReturnCode::FAILURE;
  }

  return CLIReturnCode::NONE;
}
