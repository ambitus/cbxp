cmake_minimum_required(VERSION 3.15)

project(CBXP VERSION 0.0.3
             DESCRIPTION "A unified and standardized interface for extracting z/OS control block data."
             LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "./dist")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "./dist")

execute_process(
    COMMAND uname
    OUTPUT_VARIABLE uname_output
    RESULT_VARIABLE uname_result
)

# ============================================================================
# Compiler Info
# ============================================================================
set(CXX_STANDARD 17)

set(
    COMPILE_OPTIONS
    -O2
    -fzos-le-char-mode=ascii
    -fstack-protector-strong
    -Wno-trigraphs
)

set(LINK_OPTIONS -m64 -Wl,-b,edit=no)

if(uname_output STREQUAL "OS/390\n")
    set(CMAKE_CXX_COMPILER "ibm-clang++64")
else()
    set(CMAKE_CXX_COMPILER "clang")
endif()

# ============================================================================
# Sources
# ============================================================================
file(
    GLOB CBXP_SRC_LIB
    "cbxp/cbxp.cpp"
    "cbxp/control_block_explorer.cpp"
    "cbxp/logger.cpp"
    "cbxp/control_blocks/*.cpp"
)

file(
    GLOB CBXP_SRC_ALL
    "cbxp/*.cpp"
    "cbxp/*.hpp"
    "cbxp/*.c"
    "cbxp/*.h"
    "cbxp/**/*.cpp"
    "cbxp/**/*.hpp"
    "cbxp/**/*.c"
    "cbxp/**/*.h"
)

# ============================================================================
# Build '/lib/libcbxp.a'
# ============================================================================
add_library(libcbxp STATIC ${CBXP_SRC_LIB})
set_target_properties(libcbxp PROPERTIES OUTPUT_NAME "cbxp")

target_include_directories(
    libcbxp PUBLIC
    cbxp
    cbxp/control_blocks
    externals
    /usr/include/zos
)

target_compile_options(libcbxp PUBLIC ${COMPILE_OPTIONS})
target_link_options(libcbxp PUBLIC ${LINK_OPTIONS})

# ============================================================================
# Build '/bin/cbxp'
# ============================================================================
add_executable(cbxp "cbxp/main.cpp")

target_link_libraries(cbxp libcbxp)

target_include_directories(cbxp PUBLIC cbxp externals)

target_compile_definitions(
    cbxp PUBLIC
    VERSION="${CMAKE_PROJECT_VERSION}"
)

target_compile_options(cbxp PUBLIC ${COMPILE_OPTIONS})
target_link_options(cbxp PUBLIC ${LINK_OPTIONS})

# ============================================================================
# Packaging
# ============================================================================
add_custom_target(
    package
    COMMAND "mkdir"
                "-p"
                "cbxp-${CMAKE_PROJECT_VERSION}/bin"
                "cbxp-${CMAKE_PROJECT_VERSION}/lib"
                "cbxp-${CMAKE_PROJECT_VERSION}/include"
    COMMAND "cp"
                "LICENSE"
                "cbxp-${CMAKE_PROJECT_VERSION}/"
    COMMAND "cp"
                "NOTICES"
                "cbxp-${CMAKE_PROJECT_VERSION}/"
    COMMAND "cp"
                "./dist/cbxp"
                "cbxp-${CMAKE_PROJECT_VERSION}/bin/"
    COMMAND "cp"
                "./dist/libcbxp.a"
                "cbxp-${CMAKE_PROJECT_VERSION}/lib/"
    COMMAND "cp"
                "./cbxp/cbxp.h"
                "cbxp-${CMAKE_PROJECT_VERSION}/include/"
    COMMAND "pax"
                "-x pax"
                "-wzvf"
                "./dist/cbxp-${CMAKE_PROJECT_VERSION}.pax.Z"
                "-o saveext"
                "cbxp-${CMAKE_PROJECT_VERSION}/*"
    DEPENDS libcbxp cbxp
)

# ============================================================================
# Cppcheck
# ============================================================================
add_custom_target(
    check
    COMMAND "cppcheck"
                "--suppress='missingIncludeSystem'"
                "--inline-suppr"
                "--language=c++"
                "--std=c++${CXX_STANDARD}"
                "--enable=all"
                "--force"
                "--check-level=exhaustive"
                "--inconclusive"
                "--error-exitcode=1"
                "-D __ptr32="
                "-D VERSION=\\\"${CMAKE_PROJECT_VERSION}\\\""
                "-I${CMAKE_CURRENT_SOURCE_DIR}/cbxp"
                "-I${CMAKE_CURRENT_SOURCE_DIR}/cbxp/python"
                "${CMAKE_CURRENT_SOURCE_DIR}/cbxp"
)

# ============================================================================
# Lint
# ============================================================================
add_custom_target(
    lint
    COMMAND "clang-format"
                "-Werror"
                "--dry-run"
                ${CBXP_SRC_ALL}
)

# ============================================================================
# Format
# ============================================================================
add_custom_target(
    format
    COMMAND "clang-format"
                "-i"
                ${CBXP_SRC_ALL}
)

# ============================================================================
# Test
# ============================================================================
add_custom_target(
    test
    COMMAND "./tests/test.sh"
    DEPENDS libcbxp cbxp
)
