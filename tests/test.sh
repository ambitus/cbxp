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

# exit code 0 
run_with_expected_exit_code 0 ./dist/cbxp ascb  
run_with_expected_exit_code 0 ./dist/cbxp asvt  
run_with_expected_exit_code 0 ./dist/cbxp cvt 
run_with_expected_exit_code 0 ./dist/cbxp ecvt  
run_with_expected_exit_code 0 ./dist/cbxp psa 
run_with_expected_exit_code 0 ./dist/cbxp -i cvt psa  
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

# exit code 255
run_with_expected_exit_code 255 ./dist/cbxp 
run_with_expected_exit_code 255 ./dist/cbxp -x "unknown flag" cvt
run_with_expected_exit_code 255 ./dist/cbxp unknown
run_with_expected_exit_code 255 ./dist/cbxp -i asvt,ascb cvt 
run_with_expected_exit_code 255 ./dist/cbxp -i asvt,as cvt 
run_with_expected_exit_code 255 ./dist/cbxp -i asvt.ascb psa 
run_with_expected_exit_code 255 ./dist/cbxp -i ascb psa 
run_with_expected_exit_code 255 ./dist/cbxp -i ecvt ascb 
run_with_expected_exit_code 255 ./dist/cbxp -i cvt.ecvt -i cvt.ascb psa 
run_with_expected_exit_code 255 ./dist/cbxp -i cvt.asvt.ascb -i ecvt psa 
run_with_expected_exit_code 255 ./dist/cbxp -i cvt cvt 
run_with_expected_exit_code 255 ./dist/cbxp -i cvt 
run_with_expected_exit_code 255 ./dist/cbxp -i -i cvt psa

echo " -------------------------------- "
echo " -------------------------------- "
echo " All tests completed successfully"
echo " -------------------------------- "
echo " -------------------------------- "
