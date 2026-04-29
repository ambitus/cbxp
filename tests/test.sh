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
run_with_expected_exit_code 0 ./dist/cbxp explore psa
run_with_expected_exit_code 0 ./dist/cbxp explore cvt
run_with_expected_exit_code 0 ./dist/cbxp explore ecvt
run_with_expected_exit_code 0 ./dist/cbxp explore asvt
run_with_expected_exit_code 0 ./dist/cbxp explore ascb
run_with_expected_exit_code 0 ./dist/cbxp explore assb
run_with_expected_exit_code 0 ./dist/cbxp explore oucb

# Format
run_with_expected_exit_code 0 ./dist/cbxp format -F tests/samples/ascb.bin ascb
run_with_expected_exit_code 0 cat -B tests/samples/cvt.bin | ./dist/cbxp format cvt
run_with_expected_exit_code 0 ./dist/cbxp format -F tests/samples/oucb.bin oucb
run_with_expected_exit_code 0 ./dist/cbxp format -F tests/samples/ascboffset40.bin -o 0x40 ascb
run_with_expected_exit_code 0 ./dist/cbxp format -F tests/samples/ascboffset40.bin -o 64 ascb
run_with_expected_exit_code 0 cat -B tests/samples/oucboffset3A8.bin | ./dist/cbxp format -o 0x3A8 oucb
run_with_expected_exit_code 0 cat -B tests/samples/oucboffset3A8.bin | ./dist/cbxp format -o 936 oucb

# Include Patterns
run_with_expected_exit_code 0 ./dist/cbxp explore -i cvt psa
run_with_expected_exit_code 0 ./dist/cbxp explore --include cvt psa
run_with_expected_exit_code 0 ./dist/cbxp explore -i ecvt cvt
run_with_expected_exit_code 0 ./dist/cbxp explore -i asvt cvt
run_with_expected_exit_code 0 ./dist/cbxp explore -i ascb asvt
run_with_expected_exit_code 0 ./dist/cbxp explore -i cvt.ecvt psa
run_with_expected_exit_code 0 ./dist/cbxp explore -i cvt.asvt.ascb psa
run_with_expected_exit_code 0 ./dist/cbxp explore -i asvt.ascb cvt
run_with_expected_exit_code 0 ./dist/cbxp explore -i ecvt -i asvt cvt
run_with_expected_exit_code 0 ./dist/cbxp explore -i cvt.ecvt -i cvt.asvt.ascb psa
run_with_expected_exit_code 0 ./dist/cbxp explore -i "cvt.**" psa
run_with_expected_exit_code 0 ./dist/cbxp explore -i "cvt.*" psa
run_with_expected_exit_code 0 ./dist/cbxp explore -i "asvt.*" -i "*" cvt
run_with_expected_exit_code 0 ./dist/cbxp explore -i assb ascb
run_with_expected_exit_code 0 ./dist/cbxp explore -i oucb ascb
run_with_expected_exit_code 0 ./dist/cbxp explore -i cvt.ecvt -i cvt.asvt.ascb.assb psa
run_with_expected_exit_code 0 ./dist/cbxp explore -i cvt.ecvt -i cvt.asvt.ascb.oucb psa

# Filters
run_with_expected_exit_code 0 ./dist/cbxp explore -f psapsa=PSA psa
run_with_expected_exit_code 0 ./dist/cbxp explore -f "cvt.asvt.ascb.assb.assbjbns=*MASTER*" -i "**" psa
run_with_expected_exit_code 0 ./dist/cbxp explore -f "cvt.asvt.ascb.assb.assbjbns=*MASTER*" -i cvt.asvt.ascb.assb psa
run_with_expected_exit_code 0 ./dist/cbxp explore -f "cvt.asvt.ascb.assb.assbjbns=*MASTER*" -f "cvt.asvt.ascb.ascbasid>0" -i "**" psa
run_with_expected_exit_code 0 ./dist/cbxp explore -f "cvt.asvt.ascb.assb.assbjbns=?MAS?ER?" -i cvt.asvt.ascb.assb psa
run_with_expected_exit_code 0 ./dist/cbxp explore -f "cvt.asvt.ascb.ascbasid=1" -i "**" psa
run_with_expected_exit_code 0 ./dist/cbxp explore -f "cvt.asvt.ascb.ascbasid>0" -i "**" psa
run_with_expected_exit_code 0 ./dist/cbxp explore -f "cvt.asvt.ascb.ascbasid<2" -i "**" psa
run_with_expected_exit_code 0 ./dist/cbxp explore -f "cvt.asvt.ascb.ascbasid>=1" -i "**" psa
run_with_expected_exit_code 0 ./dist/cbxp explore -f "cvt.asvt.ascb.ascbasid<=2" -i "**" psa
run_with_expected_exit_code 0 ./dist/cbxp explore -f "cvtasmvt=2281701376" cvt
run_with_expected_exit_code 0 ./dist/cbxp explore -f "cvtasmvt=0x88000000" cvt
run_with_expected_exit_code 0 ./dist/cbxp explore -f "cvtasmvt>0x87FFFFFF" cvt
run_with_expected_exit_code 0 ./dist/cbxp explore -f "cvtasmvt<0x88000001" cvt
run_with_expected_exit_code 0 ./dist/cbxp explore -f "cvtasmvt>=0x87FFFFFF" cvt
run_with_expected_exit_code 0 ./dist/cbxp explore -f "cvtasmvt<=0x88000000" cvt
run_with_expected_exit_code 0 ./dist/cbxp explore -f 'oucbtrxn=OMVS' oucb
run_with_expected_exit_code 0 ./dist/cbxp explore -i oucb -f 'oucb.oucbtrxn=OMVS' ascb
run_with_expected_null_response ./dist/cbxp explore -f psapsa=PSB psa
run_with_expected_null_response ./dist/cbxp explore -f "ascb.assb.assbjbns=*MASTER*" -f "ascb.ascbasid=2" -i ascb.assb asvt
run_with_expected_exit_code 0 ./dist/cbxp explore -f assbjbns="*MASTER*" -f assbjbni=  assb
run_with_expected_exit_code 0 ./dist/cbxp explore -f assbjbns="*MASTER*" -f assbjbni="" assb
run_with_expected_exit_code 0 ./dist/cbxp explore -f assbjbns="*MASTER*" -f assbjbni='' assb


# Debug Mode
run_with_expected_exit_code 0 ./dist/cbxp explore -d psa
run_with_expected_exit_code 0 ./dist/cbxp explore --debug psa
# Show Usage
run_with_expected_exit_code 0 ./dist/cbxp -h
run_with_expected_exit_code 0 ./dist/cbxp --help
# Show Version
run_with_expected_exit_code 0 ./dist/cbxp -v
run_with_expected_exit_code 0 ./dist/cbxp --version

# Errors: Bad Usage
run_with_expected_exit_code 255 ./dist/cbxp explore
run_with_expected_exit_code 255 ./dist/cbxp explore -x "unknown flag" cvt
run_with_expected_exit_code 255 ./dist/cbxp explore -i cvt
run_with_expected_exit_code 255 ./dist/cbxp explore -i -i cvt psa
run_with_expected_exit_code 255 ./dist/cbxp explore -d -d psa
run_with_expected_exit_code 255 ./dist/cbxp explore -f psa
run_with_expected_exit_code 255 ./dist/cbxp explore -f psapsa=psa
run_with_expected_exit_code 255 ./dist/cbxp explore --debug -d psa
run_with_expected_exit_code 255 ./dist/cbxp explore -F tests/samples/ascb.bin psa
run_with_expected_exit_code 255 ./dist/cbxp explore -o 1 psa
run_with_expected_exit_code 255 ./dist/cbxp format
run_with_expected_exit_code 255 ./dist/cbxp format psa
run_with_expected_exit_code 255 ./dist/cbxp format -i cvt psa
run_with_expected_exit_code 255 ./dist/cbxp format -f psapsa=PSA psa
run_with_expected_exit_code 255 ./dist/cbxp format -F tests/samples/ascb.bin -o 999999 ascb
# Errors: Unknown Control Block
run_with_expected_exit_code 255 ./dist/cbxp explore unknown
# Errors: Bad Include Patterns
run_with_expected_exit_code 255 ./dist/cbxp explore -i asvt,ascb cvt
run_with_expected_exit_code 255 ./dist/cbxp explore -i asvt,as cvt
run_with_expected_exit_code 255 ./dist/cbxp explore -i asvt.ascb psa
run_with_expected_exit_code 255 ./dist/cbxp explore -i ascb psa
run_with_expected_exit_code 255 ./dist/cbxp explore -i ecvt ascb
run_with_expected_exit_code 255 ./dist/cbxp explore -i cvt.ecvt -i cvt.ascb psa
run_with_expected_exit_code 255 ./dist/cbxp explore -i cvt.asvt.ascb -i ecvt psa
run_with_expected_exit_code 255 ./dist/cbxp explore -i cvt cvt
# Errors: Bad Filters
run_with_expected_exit_code 255 ./dist/cbxp explore -f "cvt.asvt.ascb.assb.assbjbns=*master*" psa
run_with_expected_exit_code 255 ./dist/cbxp explore -f "cvt.asvt.ascb.assb.assbjbns<*master*" -i "**" psa
run_with_expected_exit_code 255 ./dist/cbxp explore -f psapsb=PSA psa
run_with_expected_exit_code 255 ./dist/cbxp explore -f assbasid= assb
run_with_expected_exit_code 255 ./dist/cbxp explore -f 'ascbasid<=junk' ascb
run_with_expected_exit_code 255 ./dist/cbxp explore -f "psapsa=psa,cvt.asvt.ascb.ascbasid<2" cvt
run_with_expected_exit_code 255 ./dist/cbxp explore -f junk psa
# Errors: Error Opening File
run_with_expected_exit_code 255 ./dist/cbxp format -F tests/samples/notreal.bin psa
# Errors: File and STDIN
run_with_expected_exit_code 255 sh -c 'cat -B tests/samples/cvt.bin | ./dist/cbxp format -F tests/samples/cvt.bin cvt'
# Errors: Buffer Too Small
run_with_expected_exit_code 255 ./dist/cbxp format -F tests/samples/ascb.bin psa

echo " -------------------------------- "
echo " -------------------------------- "
echo " All tests completed successfully"
echo " -------------------------------- "
echo " -------------------------------- "
