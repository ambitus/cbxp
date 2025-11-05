#!/bin/bash

run_with_expected_exit_code() {
    expected_code=$1
    shift
    echo "Running: $* (expected exit code: $expected_code)"

    "$@" > /dev/null
    return_code=$?

    if [ $return_code -eq $expected_code ]; then
        echo "Command exited with $return_code as expected."
    else
        echo "Unexpected exit code: got $return_code, expected $expected_code" 
        exit 1
    fi
    echo
}

# Basic Usage
run_with_expected_exit_code 0 ./dist/cbxp psa
run_with_expected_exit_code 0 ./dist/cbxp cvt
run_with_expected_exit_code 0 ./dist/cbxp ecvt
run_with_expected_exit_code 0 ./dist/cbxp asvt
run_with_expected_exit_code 0 ./dist/cbxp ascb
# Include Patterns
run_with_expected_exit_code 0 ./dist/cbxp -i cvt psa
run_with_expected_exit_code 0 ./dist/cbxp --include cvt psa
run_with_expected_exit_code 0 ./dist/cbxp -i ecvt cvt
run_with_expected_exit_code 0 ./dist/cbxp -i asvt cvt
run_with_expected_exit_code 0 ./dist/cbxp -i ascb asvt
run_with_expected_exit_code 0 ./dist/cbxp -i cvt.ecvt psa
run_with_expected_exit_code 0 ./dist/cbxp -i cvt.asvt.ascb psa
run_with_expected_exit_code 0 ./dist/cbxp -i asvt.ascb cvt
run_with_expected_exit_code 0 ./dist/cbxp -i ecvt -i asvt cvt
run_with_expected_exit_code 0 ./dist/cbxp -i cvt.ecvt -i cvt.asvt.ascb psa
run_with_expected_exit_code 0 ./dist/cbxp -i "cvt.**" psa
run_with_expected_exit_code 0 ./dist/cbxp -i "cvt.*" psa
run_with_expected_exit_code 0 ./dist/cbxp -i "asvt.*" -i "*" cvt
# Debug Mode
run_with_expected_exit_code 0 ./dist/cbxp -d psa
run_with_expected_exit_code 0 ./dist/cbxp --debug psa
# Show Usage
run_with_expected_exit_code 0 ./dist/cbxp -h
run_with_expected_exit_code 0 ./dist/cbxp --help
# Show Version
run_with_expected_exit_code 0 ./dist/cbxp -v
run_with_expected_exit_code 0 ./dist/cbxp --version

# Errors: Bad Usage
run_with_expected_exit_code 255 ./dist/cbxp 
run_with_expected_exit_code 255 ./dist/cbxp -x "unknown flag" cvt
run_with_expected_exit_code 255 ./dist/cbxp -i cvt 
run_with_expected_exit_code 255 ./dist/cbxp -i -i cvt psa
# Errors: Unknown Control Block
run_with_expected_exit_code 255 ./dist/cbxp unknown
# Errors: Bad Include Patterns
run_with_expected_exit_code 255 ./dist/cbxp -i asvt,ascb cvt
run_with_expected_exit_code 255 ./dist/cbxp -i asvt,as cvt
run_with_expected_exit_code 255 ./dist/cbxp -i asvt.ascb psa
run_with_expected_exit_code 255 ./dist/cbxp -i ascb psa
run_with_expected_exit_code 255 ./dist/cbxp -i ecvt ascb
run_with_expected_exit_code 255 ./dist/cbxp -i cvt.ecvt -i cvt.ascb psa
run_with_expected_exit_code 255 ./dist/cbxp -i cvt.asvt.ascb -i ecvt psa
run_with_expected_exit_code 255 ./dist/cbxp -i cvt cvt

echo " -------------------------------- "
echo " -------------------------------- "
echo " All tests completed successfully"
echo " -------------------------------- "
echo " -------------------------------- "
