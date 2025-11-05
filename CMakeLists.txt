cmake_minimum_required(VERSION 3.15)

execute_process(
    COMMAND uname
    OUTPUT_VARIABLE uname_output
    RESULT_VARIABLE uname_result
)

set(CXX_STANDARD 17)

if(uname_output STREQUAL "OS/390\n")
    set(CMAKE_CXX_COMPILER "ibm-clang++64")
else()
    set(CMAKE_CXX_COMPILER "clang")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "./dist")

project(CBXP VERSION 0.0.2
             DESCRIPTION "A unified and standardized interface for extracting z/OS control block data."
             LANGUAGES C CXX)

file(
    GLOB CBXP_SRC 
    "cbxp/*.cpp"
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

add_executable(cbxp ${CBXP_SRC})

target_include_directories(
    cbxp PUBLIC
    cbxp
    cbxp/control_blocks
    externals
    /usr/include/zos
)

target_compile_definitions(
    cbxp
    PUBLIC
    VERSION="${CMAKE_PROJECT_VERSION}"
)

target_compile_options(
    cbxp PUBLIC 
    -fzos-le-char-mode=ascii
    -Wno-trigraphs
)

add_custom_target(
    package
    COMMAND "mkdir"
                "-p"
                "cbxp-${CMAKE_PROJECT_VERSION}/bin"
    COMMAND "cp"
                "LICENSE"
                "cbxp-${CMAKE_PROJECT_VERSION}/"
    COMMAND "cp"
                "NOTICES"
                "cbxp-${CMAKE_PROJECT_VERSION}/"
    COMMAND "cp"
                "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cbxp"
                "cbxp-${CMAKE_PROJECT_VERSION}/bin/"
    COMMAND "pax"
                "-wzf"
                "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cbxp-${CMAKE_PROJECT_VERSION}.pax.Z"
                "-o saveext"
                "cbxp-${CMAKE_PROJECT_VERSION}/*"
    DEPENDS cbxp
)

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

add_custom_target(
    lint
    COMMAND "clang-format"
                "-Werror"
                "--dry-run"
                ${CBXP_SRC_ALL}
)

add_custom_target(
    format
    COMMAND "clang-format"
                "-i"
                ${CBXP_SRC_ALL}
)

add_custom_target(
    test
    COMMAND "./tests/test.sh"
    DEPENDS cbxp
)
