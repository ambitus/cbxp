# =============================================================================
# CTest suite
#
# Conventions used:
#   * Tests that expect exit code 0      -> plain add_test (CTest default pass)
#   * Tests that expect exit code 255    -> add_test + WILL_FAIL TRUE
#   * Tests that expect "null" on stdout -> add_test + PASS_REGULAR_EXPRESSION "null"
#
# The cbxp executable is referenced via the generator expression
# $<TARGET_FILE:cbxp> so the path is always correct regardless of the
# build-output directory.
# =============================================================================

set(EXE $<TARGET_FILE:cbxp>)
set(SAMPLES ${CMAKE_SOURCE_DIR}/tests/samples)

# ---------------------------------------------------------------------------
# Helper macros - "cbxp.test" prefix is prepended to every test name
#
# ---------------------------------------------------------------------------
macro(cbxp_test_success name)
    add_test(NAME cbxp.test${name} COMMAND ${ARGN})
endmacro()

# Helper macro - tests that MUST exit non-zero (255)
macro(cbxp_test_fail name)
    add_test(NAME cbxp.test${name} COMMAND ${ARGN})
    set_property(TEST cbxp.test${name} PROPERTY WILL_FAIL TRUE)
endmacro()

# Helper macro - tests whose stdout must contain "null"
macro(cbxp_test_null name)
    add_test(NAME cbxp.test${name} COMMAND ${ARGN})
    set_property(TEST cbxp.test${name} PROPERTY PASS_REGULAR_EXPRESSION "null")
endmacro()

# =============================================================================
# General
# =============================================================================
cbxp_test_success(GeneralHelpShort          ${EXE} -h)
cbxp_test_success(GeneralHelpLong           ${EXE} --help)
cbxp_test_success(GeneralVersionShort       ${EXE} -v)
cbxp_test_success(GeneralVersionLong        ${EXE} --version)
cbxp_test_fail(GeneralErrorBadArgJunk       ${EXE} junk)
cbxp_test_fail(GeneralErrorBadFlagJunk      ${EXE} --junk)

# =============================================================================
# Extract - Show Usage
# =============================================================================
cbxp_test_success(ExtractHelpLong           ${EXE} extract --help)
cbxp_test_success(ExtractHelpShort          ${EXE} extract -h)

# =============================================================================
# Extract - Basic Usage
# =============================================================================
cbxp_test_success(ExtractPsa                ${EXE} extract psa)
cbxp_test_success(ExtractCvt                ${EXE} extract cvt)
cbxp_test_success(ExtractEcvt               ${EXE} extract ecvt)
cbxp_test_success(ExtractAsvt               ${EXE} extract asvt)
cbxp_test_success(ExtractAscb               ${EXE} extract ascb)
cbxp_test_success(ExtractAssb               ${EXE} extract assb)
cbxp_test_success(ExtractOucb               ${EXE} extract oucb)
cbxp_test_success(ExtractLdax               ${EXE} extract ldax)

# =============================================================================
# Extract - Debug Mode
# =============================================================================
cbxp_test_success(ExtractDebugShort         ${EXE} extract -d psa)
cbxp_test_success(ExtractDebugLong          ${EXE} extract --debug psa)

# =============================================================================
# Extract - Include Patterns
# =============================================================================
cbxp_test_success(ExtractIncludeCvtPsa                         ${EXE} extract -i cvt psa)
cbxp_test_success(ExtractIncludeCvtPsaLong                     ${EXE} extract --include cvt psa)
cbxp_test_success(ExtractIncludeEcvtCvt                        ${EXE} extract -i ecvt cvt)
cbxp_test_success(ExtractIncludeAsvtCvt                        ${EXE} extract -i asvt cvt)
cbxp_test_success(ExtractIncludeAscbAsvt                       ${EXE} extract -i ascb asvt)
cbxp_test_success(ExtractIncludeCvtEcvtPsa                     ${EXE} extract -i cvt.ecvt psa)
cbxp_test_success(ExtractIncludeCvtAsvtAscbPsa                 ${EXE} extract -i cvt.asvt.ascb psa)
cbxp_test_success(ExtractIncludeAsvtAscbCvt                    ${EXE} extract -i asvt.ascb cvt)
cbxp_test_success(ExtractIncludeMultiEcvtAsvtCvt               ${EXE} extract -i ecvt -i asvt cvt)
cbxp_test_success(ExtractIncludeMultiCvtEcvtCvtAsvtAscbPsa     ${EXE} extract -i cvt.ecvt -i cvt.asvt.ascb psa)
cbxp_test_success(ExtractIncludeGlobCvtDeepPsa                 ${EXE} extract -i "cvt.**" psa)
cbxp_test_success(ExtractIncludeGlobCvtShallowPsa              ${EXE} extract -i "cvt.*" psa)
cbxp_test_success(ExtractIncludeGlobAsvtShallowWildCvt         ${EXE} extract -i "asvt.*" -i "*" cvt)
cbxp_test_success(ExtractIncludeAssbAscb                       ${EXE} extract -i assb ascb)
cbxp_test_success(ExtractIncludeOucbAscb                       ${EXE} extract -i oucb ascb)
cbxp_test_success(ExtractIncludeCvtEcvtCvtAsvtAscbAssbPsa      ${EXE} extract -i cvt.ecvt -i cvt.asvt.ascb.assb psa)
cbxp_test_success(ExtractIncludeCvtEcvtCvtAsvtAscbOucbPsa      ${EXE} extract -i cvt.ecvt -i cvt.asvt.ascb.oucb psa)
cbxp_test_success(ExtractIncludeGlobWildAssb                   ${EXE} extract -i "*" assb)
cbxp_test_success(ExtractIncludeGlobDeepWildAssb               ${EXE} extract -i "**" assb)
cbxp_test_success(ExtractIncludeGlobAssbDeepAscb               ${EXE} extract -i "assb.**" ascb)
cbxp_test_success(ExtractIncludeLdaxAssb                       ${EXE} extract -i ldax assb)
cbxp_test_success(ExtractIncludeAssbLdaxAscb                   ${EXE} extract -i assb.ldax ascb)
cbxp_test_success(ExtractIncludeAscbAssbLdaxAsvt               ${EXE} extract -i ascb.assb.ldax asvt)
cbxp_test_success(ExtractIncludeCvtAsvtAscbAssbLdaxPsa         ${EXE} extract -i cvt.asvt.ascb.assb.ldax psa)

# =============================================================================
# Extract - Filters
# =============================================================================
cbxp_test_success(ExtractFilterPsapsaEqPSA                          ${EXE} extract -f psapsa=PSA psa)
cbxp_test_success(ExtractFilterAssbjbnsMASTERGlobAllPsa             ${EXE} extract -f "cvt.asvt.ascb.assb.assbjbns=*MASTER*" -i "**" psa)
cbxp_test_success(ExtractFilterAssbjbnsMASTERIncludeAssbPsa         ${EXE} extract -f "cvt.asvt.ascb.assb.assbjbns=*MASTER*" -i cvt.asvt.ascb.assb psa)
cbxp_test_success(ExtractFilterAssbjbnsMASTERAscbasidGt0GlobPsa     ${EXE} extract -f "cvt.asvt.ascb.assb.assbjbns=*MASTER*" -f "cvt.asvt.ascb.ascbasid>0" -i "**" psa)
cbxp_test_success(ExtractFilterAssbjbnsMASWildERIncludeAssbPsa      ${EXE} extract -f "cvt.asvt.ascb.assb.assbjbns=?MAS?ER?" -i cvt.asvt.ascb.assb psa)
cbxp_test_success(ExtractFilterAscbasidEq1                          ${EXE} extract -f "cvt.asvt.ascb.ascbasid=1" -i "**" psa)
cbxp_test_success(ExtractFilterAscbasidGt0                          ${EXE} extract -f "cvt.asvt.ascb.ascbasid>0" -i "**" psa)
cbxp_test_success(ExtractFilterAscbasidLt2                          ${EXE} extract -f "cvt.asvt.ascb.ascbasid<2" -i "**" psa)
cbxp_test_success(ExtractFilterAscbasidGe1                          ${EXE} extract -f "cvt.asvt.ascb.ascbasid>=1" -i "**" psa)
cbxp_test_success(ExtractFilterAscbasidLe2                          ${EXE} extract -f "cvt.asvt.ascb.ascbasid<=2" -i "**" psa)
cbxp_test_success(ExtractFilterCvtasmvtEqDecimal                    ${EXE} extract -f "cvtasmvt=2281701376" cvt)
cbxp_test_success(ExtractFilterCvtasmvtEqHex                        ${EXE} extract -f "cvtasmvt=0x88000000" cvt)
cbxp_test_success(ExtractFilterCvtasmvtGtHex                        ${EXE} extract -f "cvtasmvt>0x87FFFFFF" cvt)
cbxp_test_success(ExtractFilterCvtasmvtLtHex                        ${EXE} extract -f "cvtasmvt<0x88000001" cvt)
cbxp_test_success(ExtractFilterCvtasmvtGeHex                        ${EXE} extract -f "cvtasmvt>=0x87FFFFFF" cvt)
cbxp_test_success(ExtractFilterCvtasmvtLeHex                        ${EXE} extract -f "cvtasmvt<=0x88000000" cvt)
cbxp_test_success(ExtractFilterOucbtrxnEqOMVS                       ${EXE} extract -f "oucbtrxn=OMVS" oucb)
cbxp_test_success(ExtractFilterOucbOucbtrxnEqOMVSIncludeOucbAscb    ${EXE} extract -i oucb -f "oucb.oucbtrxn=OMVS" ascb)
cbxp_test_null(ExtractFilterNullPsapsaEqPSB                         ${EXE} extract -f psapsa=PSB psa)
cbxp_test_null(ExtractFilterNullAssbjbnsMASTERAscbasid2IncludeAsvt  ${EXE} extract -f "ascb.assb.assbjbns=*MASTER*" -f "ascb.ascbasid=2" -i ascb.assb asvt)
cbxp_test_success(ExtractFilterAssbjbnsMASTERAssbjbniEmptyBare      ${EXE} extract -f assbjbns=*MASTER* -f assbjbni=  assb)
cbxp_test_success(ExtractFilterAssbjbnsMASTERAssbjbniEmptyDquote    ${EXE} extract -f assbjbns=*MASTER* -f "assbjbni=" assb)
cbxp_test_success(ExtractFilterAssbjbnsMASTERAssbjbniEmptySquote    ${EXE} extract -f assbjbns=*MASTER* -f "assbjbni=" assb)
cbxp_test_success(ExtractFilterLdaxTcthwmEq0                        ${EXE} extract -f "cvt.asvt.ascb.assb.ldax.ldax_tcthwm=0" -i cvt.asvt.ascb.assb.ldax psa)
cbxp_test_success(ExtractFilterLdaxTcthwmEq0Ldax                    ${EXE} extract -f "ldax_tcthwm=0" ldax)
cbxp_test_null(ExtractFilterNullLdaxIdInvalid                       ${EXE} extract -f "cvt.asvt.ascb.assb.ldax.ldax_id=INVALID" -i cvt.asvt.ascb.assb.ldax psa)

# =============================================================================
# Extract - Errors: Bad Usage
# =============================================================================
cbxp_test_fail(ExtractErrorNoArgs               ${EXE} extract)
cbxp_test_fail(ExtractErrorBadFlagJunk          ${EXE} extract --junk)
cbxp_test_fail(ExtractErrorBadFlagJunkJunk      ${EXE} extract --junk junk)
cbxp_test_fail(ExtractErrorUnknownFlagX         ${EXE} extract -x "unknown flag" cvt)
cbxp_test_fail(ExtractErrorIncludeNoValue       ${EXE} extract -i cvt)
cbxp_test_fail(ExtractErrorIncludeMissingValue  ${EXE} extract -i -i cvt psa)
cbxp_test_fail(ExtractErrorFilterNoValue        ${EXE} extract -f psa)
cbxp_test_fail(ExtractErrorFilterNoBlock        ${EXE} extract -f psapsa=psa)
cbxp_test_fail(ExtractErrorBadFlagF             ${EXE} extract -F ${SAMPLES}/ascb.bin psa)
cbxp_test_fail(ExtractErrorBadFlagO             ${EXE} extract -o 1 psa)

# =============================================================================
# Extract - Errors: Unknown Control Block
# =============================================================================
cbxp_test_fail(ExtractErrorUnknownBlock         ${EXE} extract unknown)

# =============================================================================
# Extract - Errors: Bad Include Patterns
# =============================================================================
cbxp_test_fail(ExtractErrorIncludeCommaAsvtAscbCvt    ${EXE} extract -i asvt,ascb cvt)
cbxp_test_fail(ExtractErrorIncludeCommaAsvtAsCvt      ${EXE} extract -i asvt,as cvt)
cbxp_test_fail(ExtractErrorIncludeAsvtAscbPsa         ${EXE} extract -i asvt.ascb psa)
cbxp_test_fail(ExtractErrorIncludeAscbPsa             ${EXE} extract -i ascb psa)
cbxp_test_fail(ExtractErrorIncludeEcvtAscb            ${EXE} extract -i ecvt ascb)
cbxp_test_fail(ExtractErrorIncludeCvtEcvtCvtAscbPsa   ${EXE} extract -i cvt.ecvt -i cvt.ascb psa)
cbxp_test_fail(ExtractErrorIncludeCvtAsvtAscbEcvtPsa  ${EXE} extract -i cvt.asvt.ascb -i ecvt psa)
cbxp_test_fail(ExtractErrorIncludeCvtCvt              ${EXE} extract -i cvt cvt)
cbxp_test_fail(ExtractErrorIncludeLdaxPsa             ${EXE} extract -i ldax psa)
cbxp_test_fail(ExtractErrorIncludeLdaxCvt             ${EXE} extract -i ldax cvt)
cbxp_test_fail(ExtractErrorIncludeLdaxAscb            ${EXE} extract -i ldax ascb)

# =============================================================================
# Extract - Errors: Bad Filters
# =============================================================================
cbxp_test_fail(ExtractErrorFilterAssbjbnsLowercasePsa     ${EXE} extract -f "cvt.asvt.ascb.assb.assbjbns=*master*" psa)
cbxp_test_fail(ExtractErrorFilterAssbjbnsLtGlobPsa        ${EXE} extract -f "cvt.asvt.ascb.assb.assbjbns<*master*" -i "**" psa)
cbxp_test_fail(ExtractErrorFilterPsapsbEqPSA              ${EXE} extract -f psapsb=PSA psa)
cbxp_test_fail(ExtractErrorFilterAssbasidEmpty            ${EXE} extract -f assbasid= assb)
cbxp_test_fail(ExtractErrorFilterAscbasidLeJunk           ${EXE} extract -f "ascbasid<=junk" ascb)
cbxp_test_fail(ExtractErrorFilterCommaMultiFilter         ${EXE} extract -f "psapsa=psa,cvt.asvt.ascb.ascbasid<2" cvt)
cbxp_test_fail(ExtractErrorFilterJunkPsa                  ${EXE} extract -f junk psa)

# =============================================================================
# Format - Show Usage
# =============================================================================
cbxp_test_success(FormatHelpShort           ${EXE} format -h)
cbxp_test_success(FormatHelpLong            ${EXE} format --help)

# =============================================================================
# Format - Basic Usage
# =============================================================================
cbxp_test_success(FormatAscbFile            ${EXE} format -F ${SAMPLES}/ascb.bin ascb)
cbxp_test_success(FormatOucbFile            ${EXE} format -F ${SAMPLES}/oucb.bin oucb)
# pipe via shell - CTest runs commands directly, so we use
# cmake -E env + sh to emulate "cat file | cmd"
cbxp_test_success(FormatCvtPipe
    ${CMAKE_COMMAND} -E env sh -c "cat ${SAMPLES}/cvt.bin | ${EXE} format cvt")

# =============================================================================
# Format - Debug Mode
# =============================================================================
cbxp_test_success(FormatDebugShort          ${EXE} format -F ${SAMPLES}/ascb.bin -d ascb)
cbxp_test_success(FormatDebugLong           ${EXE} format -F ${SAMPLES}/ascb.bin --debug ascb)

# =============================================================================
# Format - Offset
# =============================================================================
cbxp_test_success(FormatOffsetHex           ${EXE} format -F ${SAMPLES}/ascb_offset_0x40.bin -o 0x40 ascb)
cbxp_test_success(FormatOffsetDecimal       ${EXE} format -F ${SAMPLES}/ascb_offset_0x40.bin -o 64 ascb)
cbxp_test_success(FormatOffsetOucbPipeHex
    ${CMAKE_COMMAND} -E env sh -c "cat ${SAMPLES}/oucb_offset_0x03a8.bin | ${EXE} format -o 0x03a8 oucb")
cbxp_test_success(FormatOffsetOucbPipeDecimal
    ${CMAKE_COMMAND} -E env sh -c "cat ${SAMPLES}/oucb_offset_0x03a8.bin | ${EXE} format -o 936 oucb")

# =============================================================================
# Format - Errors: Bad Usage
# =============================================================================
cbxp_test_fail(FormatErrorNoArgs              ${EXE} format)
cbxp_test_fail(FormatErrorBadFlagJunk         ${EXE} format --junk)
cbxp_test_fail(FormatErrorBadFlagJunkJunk     ${EXE} format --junk junk)
cbxp_test_fail(FormatErrorNoFilePsa           ${EXE} format psa)
cbxp_test_fail(FormatErrorFlagINotSupported   ${EXE} format -i cvt psa)
cbxp_test_fail(FormatErrorFlagFNotSupported   ${EXE} format -f psapsa=PSA psa)

# =============================================================================
# Format - Errors: Unknown Control Block
# =============================================================================
cbxp_test_fail(FormatErrorUnknownBlock        ${EXE} format -F ${SAMPLES}/ascb.bin -d unknown)

# =============================================================================
# Format - Errors: Bad Offset
# =============================================================================
cbxp_test_fail(FormatErrorOffsetTooLarge      ${EXE} format -F ${SAMPLES}/ascb.bin -o 999999 ascb)
cbxp_test_fail(FormatErrorOffsetNegative      ${EXE} format -F ${SAMPLES}/ascb.bin -o -1 ascb)
cbxp_test_fail(FormatErrorOffsetJunk          ${EXE} format -F ${SAMPLES}/ascb.bin -o JUNK ascb)
cbxp_test_fail(FormatErrorOffsetFloat         ${EXE} format -F ${SAMPLES}/ascb.bin -o 5.5 ascb)

# =============================================================================
# Format - Errors: Error Opening File
# =============================================================================
cbxp_test_fail(FormatErrorFileNotFound        ${EXE} format -F ${SAMPLES}/notreal.bin psa)

# =============================================================================
# Format - Errors: No File/Pipe provided
# =============================================================================
cbxp_test_fail(FormatErrorNoFileOrPipe        ${EXE} format psa)

# =============================================================================
# Format - Errors: File and Pipe
# =============================================================================
cbxp_test_fail(FormatErrorFileAndPipe
    ${CMAKE_COMMAND} -E env sh -c "cat ${SAMPLES}/cvt.bin | ${EXE} format -F ${SAMPLES}/cvt.bin cvt")

# =============================================================================
# Format - Errors: Data Too Small
# =============================================================================
cbxp_test_fail(FormatErrorDataTooSmall        ${EXE} format -F ${SAMPLES}/ascb.bin psa)
