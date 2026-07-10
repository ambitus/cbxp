cmake_minimum_required(VERSION 3.15)

project(CBXP VERSION 0.0.4
             DESCRIPTION "A unified and standardized interface for extracting and formatting z/OS control block data."
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
    "cbxp/*.cpp"
    "cbxp/control_blocks/*.cpp"
)

file(
    GLOB CBXP_SRC_CLI
    "cbxp/cli/*.cpp"
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
# ============================================================================
# Generate schema header from cbxp/schemas/control_block_v1.json
# ============================================================================
set(CBXP_SCHEMAS_GENERATED_DIR "${CMAKE_BINARY_DIR}/cbxp/schemas")
file(MAKE_DIRECTORY "${CBXP_SCHEMAS_GENERATED_DIR}")

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/cbxp/schemas/control_block_v1.json" CBXP_SCHEMA_JSON)

file(WRITE "${CBXP_SCHEMAS_GENERATED_DIR}/control_block_v1.hpp"
"#ifndef CBXP_SCHEMA_CONTROL_BLOCK_V1_JSON_HPP\n\
#define CBXP_SCHEMA_CONTROL_BLOCK_V1_JSON_HPP\n\
\n\
#define CBXP_SCHEMA_JSON R\"JSON(${CBXP_SCHEMA_JSON})JSON\"\n\
\n\
#endif // CBXP_SCHEMA_CONTROL_BLOCK_V1_JSON_HPP\n"
)

# ============================================================================
# Generate mapping headers from JSON files in cbxp/mappings/
# ============================================================================
set(CBXP_MAPPINGS_GENERATED_DIR "${CMAKE_BINARY_DIR}/cbxp/mappings")
file(MAKE_DIRECTORY "${CBXP_MAPPINGS_GENERATED_DIR}")

file(GLOB CBXP_MAPPING_JSON_FILES "${CMAKE_CURRENT_SOURCE_DIR}/cbxp/mappings/*.json")

set(CBXP_MAPPING_FORCED_INCLUDES "")

foreach(MAPPING_JSON_FILE ${CBXP_MAPPING_JSON_FILES})
    get_filename_component(MAPPING_NAME "${MAPPING_JSON_FILE}" NAME_WE)
    string(TOUPPER "${MAPPING_NAME}" MAPPING_NAME_UPPER)

    file(READ "${MAPPING_JSON_FILE}" MAPPING_JSON)

    set(MAPPING_GUARD "${MAPPING_NAME_UPPER}_JSON_HPP")
    set(MAPPING_MACRO "${MAPPING_NAME_UPPER}_JSON")
    set(MAPPING_HEADER_PATH "${CBXP_MAPPINGS_GENERATED_DIR}/${MAPPING_NAME}.hpp")

    file(WRITE "${MAPPING_HEADER_PATH}"
"#ifndef ${MAPPING_GUARD}\n\
#define ${MAPPING_GUARD}\n\
\n\
#define ${MAPPING_MACRO} R\"JSON(${MAPPING_JSON})JSON\"\n\
\n\
#endif // ${MAPPING_GUARD}\n"
    )

    list(APPEND CBXP_MAPPING_FORCED_INCLUDES "--include=${MAPPING_HEADER_PATH}")
endforeach()

add_library(libcbxp STATIC ${CBXP_SRC_LIB})
set_target_properties(libcbxp PROPERTIES OUTPUT_NAME "cbxp")

target_include_directories(
    libcbxp PUBLIC
    cbxp
    cbxp/control_blocks
    externals/json
    externals/json-schema-validor
    /usr/include/zos
)

target_compile_options(
    libcbxp PUBLIC
    ${COMPILE_OPTIONS}
    "--include=${CBXP_SCHEMAS_GENERATED_DIR}/control_block_v1.hpp"
    ${CBXP_MAPPING_FORCED_INCLUDES}
)
target_link_options(libcbxp PUBLIC ${LINK_OPTIONS})

# ============================================================================
# Build '/bin/cbxp'
# ============================================================================
add_executable(cbxp ${CBXP_SRC_CLI})

target_link_libraries(cbxp libcbxp)

target_include_directories(cbxp PUBLIC cbxp externals)

target_compile_definitions(
    cbxp PUBLIC
    VERSION="${CMAKE_PROJECT_VERSION}"
)

target_compile_options(cbxp PUBLIC ${COMPILE_OPTIONS})
target_link_options(cbxp PUBLIC ${LINK_OPTIONS})

# ============================================================================
# Install
# ============================================================================
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    SET(CMAKE_INSTALL_PREFIX "~/cbxp-${CMAKE_PROJECT_VERSION}" CACHE PATH "install path" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

include(GNUInstallDirs)

install(
    FILES "LICENSE" "NOTICES"
    DESTINATION ${CMAKE_INSTALL_PREFIX}
)

install(
    TARGETS cbxp
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    TARGETS libcbxp
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
    FILES "./cbxp/cbxp.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
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
