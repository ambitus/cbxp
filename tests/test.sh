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

run_with_expected_null_response() {
    echo "Running: $* (expecting null response)"

    if "$@" | grep -q -x "null"; then
        echo "Command exited with null response as expected."
    else
        echo "Unexpected non-null response" 
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
run_with_expected_exit_code 0 ./dist/cbxp assb
run_with_expected_exit_code 0 ./dist/cbxp oucb

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
run_with_expected_exit_code 0 ./dist/cbxp -i assb ascb
run_with_expected_exit_code 0 ./dist/cbxp -i oucb ascb
run_with_expected_exit_code 0 ./dist/cbxp -i cvt.ecvt -i cvt.asvt.ascb.assb psa
run_with_expected_exit_code 0 ./dist/cbxp -i cvt.ecvt -i cvt.asvt.ascb.oucb psa

# Filters
run_with_expected_exit_code 0 ./dist/cbxp -f psapsa=PSA psa
run_with_expected_exit_code 0 ./dist/cbxp -f "cvt.asvt.ascb.assb.assbjbns=*MASTER*" -i "**" psa
run_with_expected_exit_code 0 ./dist/cbxp -f "cvt.asvt.ascb.assb.assbjbns=*MASTER*" -i cvt.asvt.ascb.assb psa
run_with_expected_exit_code 0 ./dist/cbxp -f "cvt.asvt.ascb.assb.assbjbns=*MASTER*" -f "cvt.asvt.ascb.ascbasid>0" -i "**" psa
run_with_expected_exit_code 0 ./dist/cbxp -f "cvt.asvt.ascb.assb.assbjbns=?MAS?ER?" -i cvt.asvt.ascb.assb psa
run_with_expected_exit_code 0 ./dist/cbxp -f "cvt.asvt.ascb.ascbasid=1" -i "**" psa
run_with_expected_exit_code 0 ./dist/cbxp -f "cvt.asvt.ascb.ascbasid>0" -i "**" psa
run_with_expected_exit_code 0 ./dist/cbxp -f "cvt.asvt.ascb.ascbasid<2" -i "**" psa
run_with_expected_exit_code 0 ./dist/cbxp -f "cvt.asvt.ascb.ascbasid>=1" -i "**" psa
run_with_expected_exit_code 0 ./dist/cbxp -f "cvt.asvt.ascb.ascbasid<=2" -i "**" psa
run_with_expected_exit_code 0 ./dist/cbxp -f "cvtasmvt=2281701376" cvt
run_with_expected_exit_code 0 ./dist/cbxp -f "cvtasmvt=0x88000000" cvt
run_with_expected_exit_code 0 ./dist/cbxp -f "cvtasmvt>0x87FFFFFF" cvt
run_with_expected_exit_code 0 ./dist/cbxp -f "cvtasmvt<0x88000001" cvt
run_with_expected_exit_code 0 ./dist/cbxp -f "cvtasmvt>=0x87FFFFFF" cvt
run_with_expected_exit_code 0 ./dist/cbxp -f "cvtasmvt<=0x88000000" cvt
run_with_expected_exit_code 0 ./dist/cbxp -f 'oucbtrxn=OMVS' oucb
run_with_expected_exit_code 0 ./dist/cbxp -i oucb -f 'oucb.oucbtrxn=OMVS' ascb
run_with_expected_null_response ./dist/cbxp -f psapsa=PSB psa
run_with_expected_null_response ./dist/cbxp -f "ascb.assb.assbjbns=*MASTER*" -f "ascb.ascbasid=2" -i ascb.assb asvt
run_with_expected_exit_code 0 ./dist/cbxp -f assbjbns="*MASTER*" -f assbjbni=  assb
run_with_expected_exit_code 0 ./dist/cbxp -f assbjbns="*MASTER*" -f assbjbni="" assb
run_with_expected_exit_code 0 ./dist/cbxp -f assbjbns="*MASTER*" -f assbjbni='' assb


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
run_with_expected_exit_code 255 ./dist/cbxp -d -d psa
run_with_expected_exit_code 255 ./dist/cbxp -f psa
run_with_expected_exit_code 255 ./dist/cbxp -f psapsa=psa
run_with_expected_exit_code 255 ./dist/cbxp --debug -d psa
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
# Errors: Bad Filters
run_with_expected_exit_code 255 ./dist/cbxp -f "cvt.asvt.ascb.assb.assbjbns=*master*" psa
run_with_expected_exit_code 255 ./dist/cbxp -f "cvt.asvt.ascb.assb.assbjbns<*master*" -i "**" psa
run_with_expected_exit_code 255 ./dist/cbxp -f psapsb=PSA psa
run_with_expected_exit_code 255 ./dist/cbxp -f assbasid= assb
run_with_expected_exit_code 255 ./dist/cbxp -f 'ascbasid<=junk' ascb
run_with_expected_exit_code 255 ./dist/cbxp -f "psapsa=psa,cvt.asvt.ascb.ascbasid<2" cvt
run_with_expected_exit_code 255 ./dist/cbxp -f junk psa

echo " -------------------------------- "
echo " -------------------------------- "
echo " All tests completed successfully"
echo " -------------------------------- "
echo " -------------------------------- "
