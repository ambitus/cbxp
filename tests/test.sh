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

## General Test Cases

# General - Show Usage
run_with_expected_exit_code 0 ./dist/cbxp -h
run_with_expected_exit_code 0 ./dist/cbxp --help
# General - Show Version
run_with_expected_exit_code 0 ./dist/cbxp -v
run_with_expected_exit_code 0 ./dist/cbxp --version
# General - Errors: Bad Usage
run_with_expected_exit_code 255 ./dist/cbxp junk
run_with_expected_exit_code 255 ./dist/cbxp --junk

## Extract Command

# Extract - Show Usage
run_with_expected_exit_code 0 ./dist/cbxp extract --help
run_with_expected_exit_code 0 ./dist/cbxp extract -h
# Extract - Basic Usage
run_with_expected_exit_code 0 ./dist/cbxp extract psa
run_with_expected_exit_code 0 ./dist/cbxp extract cvt
run_with_expected_exit_code 0 ./dist/cbxp extract ecvt
run_with_expected_exit_code 0 ./dist/cbxp extract asvt
run_with_expected_exit_code 0 ./dist/cbxp extract ascb
run_with_expected_exit_code 0 ./dist/cbxp extract assb
run_with_expected_exit_code 0 ./dist/cbxp extract oucb
run_with_expected_exit_code 0 ./dist/cbxp extract ldax
# Extract - Debug Mode
run_with_expected_exit_code 0 ./dist/cbxp extract -d psa
run_with_expected_exit_code 0 ./dist/cbxp extract --debug psa
# Extract - Include Patterns
run_with_expected_exit_code 0 ./dist/cbxp extract -i cvt psa
run_with_expected_exit_code 0 ./dist/cbxp extract --include cvt psa
run_with_expected_exit_code 0 ./dist/cbxp extract -i ecvt cvt
run_with_expected_exit_code 0 ./dist/cbxp extract -i asvt cvt
run_with_expected_exit_code 0 ./dist/cbxp extract -i ascb asvt
run_with_expected_exit_code 0 ./dist/cbxp extract -i cvt.ecvt psa
run_with_expected_exit_code 0 ./dist/cbxp extract -i cvt.asvt.ascb psa
run_with_expected_exit_code 0 ./dist/cbxp extract -i asvt.ascb cvt
run_with_expected_exit_code 0 ./dist/cbxp extract -i ecvt -i asvt cvt
run_with_expected_exit_code 0 ./dist/cbxp extract -i cvt.ecvt -i cvt.asvt.ascb psa
run_with_expected_exit_code 0 ./dist/cbxp extract -i "cvt.**" psa
run_with_expected_exit_code 0 ./dist/cbxp extract -i "cvt.*" psa
run_with_expected_exit_code 0 ./dist/cbxp extract -i "asvt.*" -i "*" cvt
run_with_expected_exit_code 0 ./dist/cbxp extract -i assb ascb
run_with_expected_exit_code 0 ./dist/cbxp extract -i oucb ascb
run_with_expected_exit_code 0 ./dist/cbxp extract -i cvt.ecvt -i cvt.asvt.ascb.assb psa
run_with_expected_exit_code 0 ./dist/cbxp extract -i cvt.ecvt -i cvt.asvt.ascb.oucb psa
run_with_expected_exit_code 0 ./dist/cbxp extract -i "*" assb
run_with_expected_exit_code 0 ./dist/cbxp extract -i "**" assb
run_with_expected_exit_code 0 ./dist/cbxp extract -i "assb.**" ascb
run_with_expected_exit_code 0 ./dist/cbxp extract -i ldax assb
run_with_expected_exit_code 0 ./dist/cbxp extract -i assb.ldax ascb
run_with_expected_exit_code 0 ./dist/cbxp extract -i ascb.assb.ldax asvt
run_with_expected_exit_code 0 ./dist/cbxp extract -i cvt.asvt.ascb.assb.ldax psa
# Extract - Filters
run_with_expected_exit_code 0 ./dist/cbxp extract -f psapsa=PSA psa
run_with_expected_exit_code 0 ./dist/cbxp extract -f "cvt.asvt.ascb.assb.assbjbns=*MASTER*" -i "**" psa
run_with_expected_exit_code 0 ./dist/cbxp extract -f "cvt.asvt.ascb.assb.assbjbns=*MASTER*" -i cvt.asvt.ascb.assb psa
run_with_expected_exit_code 0 ./dist/cbxp extract -f "cvt.asvt.ascb.assb.assbjbns=*MASTER*" -f "cvt.asvt.ascb.ascbasid>0" -i "**" psa
run_with_expected_exit_code 0 ./dist/cbxp extract -f "cvt.asvt.ascb.assb.assbjbns=?MAS?ER?" -i cvt.asvt.ascb.assb psa
run_with_expected_exit_code 0 ./dist/cbxp extract -f "cvt.asvt.ascb.ascbasid=1" -i "**" psa
run_with_expected_exit_code 0 ./dist/cbxp extract -f "cvt.asvt.ascb.ascbasid>0" -i "**" psa
run_with_expected_exit_code 0 ./dist/cbxp extract -f "cvt.asvt.ascb.ascbasid<2" -i "**" psa
run_with_expected_exit_code 0 ./dist/cbxp extract -f "cvt.asvt.ascb.ascbasid>=1" -i "**" psa
run_with_expected_exit_code 0 ./dist/cbxp extract -f "cvt.asvt.ascb.ascbasid<=2" -i "**" psa
run_with_expected_exit_code 0 ./dist/cbxp extract -f "cvtasmvt=2281701376" cvt
run_with_expected_exit_code 0 ./dist/cbxp extract -f "cvtasmvt=0x88000000" cvt
run_with_expected_exit_code 0 ./dist/cbxp extract -f "cvtasmvt>0x87FFFFFF" cvt
run_with_expected_exit_code 0 ./dist/cbxp extract -f "cvtasmvt<0x88000001" cvt
run_with_expected_exit_code 0 ./dist/cbxp extract -f "cvtasmvt>=0x87FFFFFF" cvt
run_with_expected_exit_code 0 ./dist/cbxp extract -f "cvtasmvt<=0x88000000" cvt
run_with_expected_exit_code 0 ./dist/cbxp extract -f 'oucbtrxn=OMVS' oucb
run_with_expected_exit_code 0 ./dist/cbxp extract -i oucb -f 'oucb.oucbtrxn=OMVS' ascb
run_with_expected_null_response ./dist/cbxp extract -f psapsa=PSB psa
run_with_expected_null_response ./dist/cbxp extract -f "ascb.assb.assbjbns=*MASTER*" -f "ascb.ascbasid=2" -i ascb.assb asvt
run_with_expected_exit_code 0 ./dist/cbxp extract -f assbjbns="*MASTER*" -f assbjbni=  assb
run_with_expected_exit_code 0 ./dist/cbxp extract -f assbjbns="*MASTER*" -f assbjbni="" assb
run_with_expected_exit_code 0 ./dist/cbxp extract -f assbjbns="*MASTER*" -f assbjbni='' assb
run_with_expected_exit_code 0 ./dist/cbxp extract -f "ldax.ldax_ldasiza=11505664" -i ldax assb
run_with_expected_exit_code 0 ./dist/cbxp extract -f "ascb.assb.ldax.ldax_ldasiza=11505664" -i ascb.assb.ldax asvt
run_with_expected_exit_code 0 ./dist/cbxp extract -f "cvt.asvt.ascb.assb.ldax.ldax_tcthwm=348160" -i cvt.asvt.ascb.assb.ldax psa
run_with_expected_exit_code 0 ./dist/cbxp extract -f "ldax_tcthwm=348160" ldax
run_with_expected_null_response ./dist/cbxp extract -f "cvt.asvt.ascb.assb.ldax.ldax_id=INVALID" -i cvt.asvt.ascb.assb.ldax psa
# Extract - Errors: Bad Usage
run_with_expected_exit_code 255 ./dist/cbxp extract
run_with_expected_exit_code 255 ./dist/cbxp extract --junk
run_with_expected_exit_code 255 ./dist/cbxp extract --junk junk
run_with_expected_exit_code 255 ./dist/cbxp extract -x "unknown flag" cvt
run_with_expected_exit_code 255 ./dist/cbxp extract -i cvt
run_with_expected_exit_code 255 ./dist/cbxp extract -i -i cvt psa
run_with_expected_exit_code 255 ./dist/cbxp extract -f psa
run_with_expected_exit_code 255 ./dist/cbxp extract -f psapsa=psa
run_with_expected_exit_code 255 ./dist/cbxp extract -F tests/samples/ascb.bin psa
run_with_expected_exit_code 255 ./dist/cbxp extract -o 1 psa
# Extract - Errors: Unknown Control Block
run_with_expected_exit_code 255 ./dist/cbxp extract unknown
# Extract - Errors: Bad Include Patterns
run_with_expected_exit_code 255 ./dist/cbxp extract -i asvt,ascb cvt
run_with_expected_exit_code 255 ./dist/cbxp extract -i asvt,as cvt
run_with_expected_exit_code 255 ./dist/cbxp extract -i asvt.ascb psa
run_with_expected_exit_code 255 ./dist/cbxp extract -i ascb psa
run_with_expected_exit_code 255 ./dist/cbxp extract -i ecvt ascb
run_with_expected_exit_code 255 ./dist/cbxp extract -i cvt.ecvt -i cvt.ascb psa
run_with_expected_exit_code 255 ./dist/cbxp extract -i cvt.asvt.ascb -i ecvt psa
run_with_expected_exit_code 255 ./dist/cbxp extract -i cvt cvt
run_with_expected_exit_code 255 ./dist/cbxp extract -i ldax psa
run_with_expected_exit_code 255 ./dist/cbxp extract -i ldax cvt
run_with_expected_exit_code 255 ./dist/cbxp extract -i ldax ascb
# Extract - Errors: Bad Filters
run_with_expected_exit_code 255 ./dist/cbxp extract -f "cvt.asvt.ascb.assb.assbjbns=*master*" psa
run_with_expected_exit_code 255 ./dist/cbxp extract -f "cvt.asvt.ascb.assb.assbjbns<*master*" -i "**" psa
run_with_expected_exit_code 255 ./dist/cbxp extract -f psapsb=PSA psa
run_with_expected_exit_code 255 ./dist/cbxp extract -f assbasid= assb
run_with_expected_exit_code 255 ./dist/cbxp extract -f 'ascbasid<=junk' ascb
run_with_expected_exit_code 255 ./dist/cbxp extract -f "psapsa=psa,cvt.asvt.ascb.ascbasid<2" cvt
run_with_expected_exit_code 255 ./dist/cbxp extract -f junk psa

## Format Command

# Format - Show Usage
run_with_expected_exit_code 0 ./dist/cbxp format -h
run_with_expected_exit_code 0 ./dist/cbxp format --help
# Format - Basic Usage
run_with_expected_exit_code 0 ./dist/cbxp format -F tests/samples/ascb.bin ascb
run_with_expected_exit_code 0 cat tests/samples/cvt.bin | ./dist/cbxp format cvt
run_with_expected_exit_code 0 ./dist/cbxp format -F tests/samples/oucb.bin oucb
# Format - Debug Mode
run_with_expected_exit_code 0 ./dist/cbxp format -F tests/samples/ascb.bin -d ascb
run_with_expected_exit_code 0 ./dist/cbxp format -F tests/samples/ascb.bin --debug ascb
# Format - Offset
run_with_expected_exit_code 0 ./dist/cbxp format -F tests/samples/ascboffset40.bin -o 0x40 ascb
run_with_expected_exit_code 0 ./dist/cbxp format -F tests/samples/ascboffset40.bin -o 64 ascb
run_with_expected_exit_code 0 cat tests/samples/oucboffset3A8.bin | ./dist/cbxp format -o 0x3A8 oucb
run_with_expected_exit_code 0 cat tests/samples/oucboffset3A8.bin | ./dist/cbxp format -o 936 oucb
# Format - Errors: Bad Usage
run_with_expected_exit_code 255 ./dist/cbxp format
run_with_expected_exit_code 255 ./dist/cbxp format --junk
run_with_expected_exit_code 255 ./dist/cbxp format --junk junk
run_with_expected_exit_code 255 ./dist/cbxp format psa
run_with_expected_exit_code 255 ./dist/cbxp format -i cvt psa
run_with_expected_exit_code 255 ./dist/cbxp format -f psapsa=PSA psa
# Format - Errors: Unknown Control Block
run_with_expected_exit_code 255 ./dist/cbxp format -F tests/samples/ascb.bin -d unknown
# Format - Errors: Bad Offset
run_with_expected_exit_code 255 ./dist/cbxp format -F tests/samples/ascb.bin -o 999999 ascb
run_with_expected_exit_code 255 ./dist/cbxp format -F tests/samples/ascb.bin -o -1 ascb
run_with_expected_exit_code 255 ./dist/cbxp format -F tests/samples/ascb.bin -o JUNK ascb
run_with_expected_exit_code 255 ./dist/cbxp format -F tests/samples/ascb.bin -o 5.5 ascb
# Format - Errors: Error Opening File
run_with_expected_exit_code 255 ./dist/cbxp format -F tests/samples/notreal.bin psa
# Format - Errors: No File/Pipe provided
run_with_expected_exit_code 255 ./dist/cbxp format psa
# Format - Errors: File and Pipe
run_with_expected_exit_code 255 sh -c 'cat tests/samples/cvt.bin | ./dist/cbxp format -F tests/samples/cvt.bin cvt'
# Format - Errors: Data Too Small
run_with_expected_exit_code 255 ./dist/cbxp format -F tests/samples/ascb.bin psa

echo " -------------------------------- "
echo " -------------------------------- "
echo " All tests completed successfully"
echo " -------------------------------- "
echo " -------------------------------- "
