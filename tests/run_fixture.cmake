# =============================================================================
# run_fixture.cmake
# Helper script to run a single CBXPPATH fixture and validate its behavior.
# Passed variables:
#   EXE         - Path to the cbxp executable
#   FIXTURE_DIR - Absolute path to the fixture directory
#   SAMPLES_DIR - Absolute path to the samples directory
# =============================================================================

if(NOT EXE OR NOT FIXTURE_DIR OR NOT SAMPLES_DIR)
    message(FATAL_ERROR "Missing required variables: EXE, FIXTURE_DIR, or SAMPLES_DIR")
endif()

# 1. Parse case.conf
if(NOT EXISTS "${FIXTURE_DIR}/case.conf")
    message(FATAL_ERROR "case.conf does not exist in ${FIXTURE_DIR}")
endif()

file(STRINGS "${FIXTURE_DIR}/case.conf" CONF_LINES)

set(EXPECT "ok")
set(CBXPPATH "")
set(BLOCK "")
set(SAMPLE "")
set(COMMAND "format")
set(CONTAINS "")
set(NOT_CONTAINS "")
set(EXPECT_OUTPUT_FILE "")
set(MESSAGE "")
set(CODE "")

foreach(line IN LISTS CONF_LINES)
    # Remove comments and strip leading/trailing whitespace
    string(REGEX REPLACE "^[ \t]*#[^\n]*" "" line "${line}")
    string(STRIP "${line}" line)
    if(line STREQUAL "")
        continue()
    endif()
    
    # Match key=value
    string(REGEX MATCH "^([^=]+)=(.*)$" MATCHED "${line}")
    if(CMAKE_MATCH_1)
        set(key "${CMAKE_MATCH_1}")
        set(val "${CMAKE_MATCH_2}")
        string(STRIP "${key}" key)
        string(STRIP "${val}" val)
        
        # Strip single or double quotes
        if((val MATCHES "^'.*'$") OR (val MATCHES "^\".*\"$"))
            string(LENGTH "${val}" val_len)
            math(EXPR val_len "${val_len} - 2")
            if(val_len GREATER_EQUAL 0)
                string(SUBSTRING "${val}" 1 ${val_len} val)
            else()
                set(val "")
            endif()
        endif()
        
        set(${key} "${val}")
    endif()
endforeach()

# 2. Prepare environment and command
if(NOT "${CBXPPATH}" STREQUAL "")
    string(REPLACE "@CASE@" "${FIXTURE_DIR}" CBXPPATH "${CBXPPATH}")
    set(ENV{CBXPPATH} "${CBXPPATH}")
else()
    unset(ENV{CBXPPATH})
endif()

if("${COMMAND}" STREQUAL "extract")
    set(CMD "${EXE}" extract "${BLOCK}")
else()
    set(CMD "${EXE}" format -F "${SAMPLES_DIR}/${SAMPLE}" "${BLOCK}")
endif()

# 3. Execute process
execute_process(
    COMMAND ${CMD}
    OUTPUT_VARIABLE STDOUT_VAR
    ERROR_VARIABLE STDERR_VAR
    RESULT_VARIABLE RESULT_VAR
)

# 4. Perform validation
if("${EXPECT}" STREQUAL "ok")
    if(NOT RESULT_VAR EQUAL 0)
        message(FATAL_ERROR "Unexpected exit code: got ${RESULT_VAR}, expected 0\nStderr: ${STDERR_VAR}")
    endif()
    
    if(NOT "${CONTAINS}" STREQUAL "")
        string(FIND "${STDOUT_VAR}" "${CONTAINS}" FIND_POS)
        if(FIND_POS EQUAL -1)
            message(FATAL_ERROR "Output does not contain expected string: ${CONTAINS}\nStdout: ${STDOUT_VAR}")
        endif()
    endif()
    
    if(NOT "${NOT_CONTAINS}" STREQUAL "")
        string(FIND "${STDOUT_VAR}" "${NOT_CONTAINS}" FIND_POS)
        if(NOT FIND_POS EQUAL -1)
            message(FATAL_ERROR "Output unexpectedly contains string: ${NOT_CONTAINS}\nStdout: ${STDOUT_VAR}")
        endif()
    endif()
    
    if(NOT "${EXPECT_OUTPUT_FILE}" STREQUAL "")
        file(READ "${FIXTURE_DIR}/${EXPECT_OUTPUT_FILE}" EXPECTED_CONTENT)
        string(STRIP "${EXPECTED_CONTENT}" EXPECTED_CONTENT)
        string(STRIP "${STDOUT_VAR}" ACTUAL_CONTENT)
        
        if(NOT ACTUAL_CONTENT STREQUAL EXPECTED_CONTENT)
            message(FATAL_ERROR "Output does not match expected output file ${EXPECT_OUTPUT_FILE}\nExpected:\n${EXPECTED_CONTENT}\nGot:\n${ACTUAL_CONTENT}")
        endif()
    endif()
else()
    # Expecting failure / error (exit code 255)
    if(NOT RESULT_VAR EQUAL 255)
        message(FATAL_ERROR "Unexpected exit code: got ${RESULT_VAR}, expected 255\nStdout: ${STDOUT_VAR}\nStderr: ${STDERR_VAR}")
    endif()
    
    if(NOT "${MESSAGE}" STREQUAL "")
        string(FIND "${STDERR_VAR}" "${MESSAGE}" FIND_POS)
        if(FIND_POS EQUAL -1)
            message(FATAL_ERROR "Error output does not contain expected message: ${MESSAGE}\nStderr: ${STDERR_VAR}")
        endif()
    endif()
endif()

message(STATUS "Fixture behaved as expected.")
