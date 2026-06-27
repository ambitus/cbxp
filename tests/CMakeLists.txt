# =============================================================================
# CTest suite — translated from tests/test.sh
#
# Conventions used:
#   • Tests that expect exit code 0      → plain add_test (CTest default pass)
#   • Tests that expect exit code 255    → add_test + WILL_FAIL TRUE
#   • Tests that expect "null" on stdout → add_test + PASS_REGULAR_EXPRESSION "null"
#
# The cbxp executable is referenced via the generator expression
# $<TARGET_FILE:cbxp> so the path is always correct regardless of the
# build-output directory.
# =============================================================================

set(EXE $<TARGET_FILE:cbxp>)
set(SAMPLES ${CMAKE_SOURCE_DIR}/tests/samples)

# ---------------------------------------------------------------------------
# Helper macro – tests that MUST exit 0
# ---------------------------------------------------------------------------
macro(cbxp_test name)
    add_test(NAME ${name} COMMAND ${ARGN})
endmacro()

# Helper macro – tests that MUST exit non-zero (255)
macro(cbxp_test_fail name)
    add_test(NAME ${name} COMMAND ${ARGN})
    set_property(TEST ${name} PROPERTY WILL_FAIL TRUE)
endmacro()

# Helper macro – tests whose stdout must contain exactly "null"
macro(cbxp_test_null name)
    add_test(NAME ${name} COMMAND ${ARGN})
    set_property(TEST ${name} PROPERTY PASS_REGULAR_EXPRESSION "null")
endmacro()

# =============================================================================
# General
# =============================================================================

cbxp_test(general.help_short          ${EXE} -h)
cbxp_test(general.help_long           ${EXE} --help)
cbxp_test(general.version_short       ${EXE} -v)
cbxp_test(general.version_long        ${EXE} --version)

cbxp_test_fail(general.error.bad_arg_junk      ${EXE} junk)
cbxp_test_fail(general.error.bad_flag_junk     ${EXE} --junk)

# =============================================================================
# Extract – Show Usage
# =============================================================================

cbxp_test(extract.help_long           ${EXE} extract --help)
cbxp_test(extract.help_short          ${EXE} extract -h)

# =============================================================================
# Extract – Basic Usage
# =============================================================================

cbxp_test(extract.psa                 ${EXE} extract psa)
cbxp_test(extract.cvt                 ${EXE} extract cvt)
cbxp_test(extract.ecvt                ${EXE} extract ecvt)
cbxp_test(extract.asvt                ${EXE} extract asvt)
cbxp_test(extract.ascb                ${EXE} extract ascb)
cbxp_test(extract.assb                ${EXE} extract assb)
cbxp_test(extract.oucb                ${EXE} extract oucb)
cbxp_test(extract.ldax                ${EXE} extract ldax)

# =============================================================================
# Extract – Debug Mode
# =============================================================================

cbxp_test(extract.debug_short         ${EXE} extract -d psa)
cbxp_test(extract.debug_long          ${EXE} extract --debug psa)

# =============================================================================
# Extract – Include Patterns
# =============================================================================

cbxp_test(extract.include.cvt_psa                          ${EXE} extract -i cvt psa)
cbxp_test(extract.include.cvt_psa_long                     ${EXE} extract --include cvt psa)
cbxp_test(extract.include.ecvt_cvt                         ${EXE} extract -i ecvt cvt)
cbxp_test(extract.include.asvt_cvt                         ${EXE} extract -i asvt cvt)
cbxp_test(extract.include.ascb_asvt                        ${EXE} extract -i ascb asvt)
cbxp_test(extract.include.cvt.ecvt_psa                     ${EXE} extract -i cvt.ecvt psa)
cbxp_test(extract.include.cvt.asvt.ascb_psa                ${EXE} extract -i cvt.asvt.ascb psa)
cbxp_test(extract.include.asvt.ascb_cvt                    ${EXE} extract -i asvt.ascb cvt)
cbxp_test(extract.include.multi_ecvt_asvt_cvt              ${EXE} extract -i ecvt -i asvt cvt)
cbxp_test(extract.include.multi_cvt.ecvt_cvt.asvt.ascb_psa ${EXE} extract -i cvt.ecvt -i cvt.asvt.ascb psa)
cbxp_test(extract.include.glob_cvt.**_psa                  ${EXE} extract -i "cvt.**" psa)
cbxp_test(extract.include.glob_cvt.*_psa                   ${EXE} extract -i "cvt.*" psa)
cbxp_test(extract.include.glob_asvt.*_*_cvt                ${EXE} extract -i "asvt.*" -i "*" cvt)
cbxp_test(extract.include.assb_ascb                        ${EXE} extract -i assb ascb)
cbxp_test(extract.include.oucb_ascb                        ${EXE} extract -i oucb ascb)
cbxp_test(extract.include.cvt.ecvt_cvt.asvt.ascb.assb_psa  ${EXE} extract -i cvt.ecvt -i cvt.asvt.ascb.assb psa)
cbxp_test(extract.include.cvt.ecvt_cvt.asvt.ascb.oucb_psa  ${EXE} extract -i cvt.ecvt -i cvt.asvt.ascb.oucb psa)
cbxp_test(extract.include.glob_*_assb                      ${EXE} extract -i "*" assb)
cbxp_test(extract.include.glob_**_assb                     ${EXE} extract -i "**" assb)
cbxp_test(extract.include.glob_assb.**_ascb                ${EXE} extract -i "assb.**" ascb)
cbxp_test(extract.include.ldax_assb                        ${EXE} extract -i ldax assb)
cbxp_test(extract.include.assb.ldax_ascb                   ${EXE} extract -i assb.ldax ascb)
cbxp_test(extract.include.ascb.assb.ldax_asvt              ${EXE} extract -i ascb.assb.ldax asvt)
cbxp_test(extract.include.cvt.asvt.ascb.assb.ldax_psa      ${EXE} extract -i cvt.asvt.ascb.assb.ldax psa)

# =============================================================================
# Extract – Filters
# =============================================================================

cbxp_test(extract.filter.psapsa_eq_PSA                           ${EXE} extract -f psapsa=PSA psa)
cbxp_test(extract.filter.assbjbns_MASTER_glob_all_psa            ${EXE} extract -f "cvt.asvt.ascb.assb.assbjbns=*MASTER*" -i "**" psa)
cbxp_test(extract.filter.assbjbns_MASTER_include_assb_psa        ${EXE} extract -f "cvt.asvt.ascb.assb.assbjbns=*MASTER*" -i cvt.asvt.ascb.assb psa)
cbxp_test(extract.filter.assbjbns_MASTER_ascbasid_gt0_glob_psa   ${EXE} extract -f "cvt.asvt.ascb.assb.assbjbns=*MASTER*" -f "cvt.asvt.ascb.ascbasid>0" -i "**" psa)
cbxp_test(extract.filter.assbjbns_?MAS?ER?_include_assb_psa      ${EXE} extract -f "cvt.asvt.ascb.assb.assbjbns=?MAS?ER?" -i cvt.asvt.ascb.assb psa)
cbxp_test(extract.filter.ascbasid_eq_1                           ${EXE} extract -f "cvt.asvt.ascb.ascbasid=1" -i "**" psa)
cbxp_test(extract.filter.ascbasid_gt_0                           ${EXE} extract -f "cvt.asvt.ascb.ascbasid>0" -i "**" psa)
cbxp_test(extract.filter.ascbasid_lt_2                           ${EXE} extract -f "cvt.asvt.ascb.ascbasid<2" -i "**" psa)
cbxp_test(extract.filter.ascbasid_ge_1                           ${EXE} extract -f "cvt.asvt.ascb.ascbasid>=1" -i "**" psa)
cbxp_test(extract.filter.ascbasid_le_2                           ${EXE} extract -f "cvt.asvt.ascb.ascbasid<=2" -i "**" psa)
cbxp_test(extract.filter.cvtasmvt_eq_decimal                     ${EXE} extract -f "cvtasmvt=2281701376" cvt)
cbxp_test(extract.filter.cvtasmvt_eq_hex                         ${EXE} extract -f "cvtasmvt=0x88000000" cvt)
cbxp_test(extract.filter.cvtasmvt_gt_hex                         ${EXE} extract -f "cvtasmvt>0x87FFFFFF" cvt)
cbxp_test(extract.filter.cvtasmvt_lt_hex                         ${EXE} extract -f "cvtasmvt<0x88000001" cvt)
cbxp_test(extract.filter.cvtasmvt_ge_hex                         ${EXE} extract -f "cvtasmvt>=0x87FFFFFF" cvt)
cbxp_test(extract.filter.cvtasmvt_le_hex                         ${EXE} extract -f "cvtasmvt<=0x88000000" cvt)
cbxp_test(extract.filter.oucbtrxn_eq_OMVS                        ${EXE} extract -f "oucbtrxn=OMVS" oucb)
cbxp_test(extract.filter.oucb.oucbtrxn_eq_OMVS_include_oucb_ascb ${EXE} extract -i oucb -f "oucb.oucbtrxn=OMVS" ascb)

cbxp_test_null(extract.filter.null.psapsa_eq_PSB                          ${EXE} extract -f psapsa=PSB psa)
cbxp_test_null(extract.filter.null.assbjbns_MASTER_ascbasid_2_include_asvt ${EXE} extract -f "ascb.assb.assbjbns=*MASTER*" -f "ascb.ascbasid=2" -i ascb.assb asvt)

cbxp_test(extract.filter.assbjbns_MASTER_assbjbni_empty_bare     ${EXE} extract -f assbjbns=*MASTER* -f assbjbni=  assb)
cbxp_test(extract.filter.assbjbns_MASTER_assbjbni_empty_dquote   ${EXE} extract -f assbjbns=*MASTER* -f "assbjbni=" assb)
cbxp_test(extract.filter.assbjbns_MASTER_assbjbni_empty_squote   ${EXE} extract -f assbjbns=*MASTER* -f "assbjbni=" assb)
cbxp_test(extract.filter.ldax_tcthwm_eq_0                        ${EXE} extract -f "cvt.asvt.ascb.assb.ldax.ldax_tcthwm=0" -i cvt.asvt.ascb.assb.ldax psa)
cbxp_test(extract.filter.ldax_tcthwm_eq_0_ldax                   ${EXE} extract -f "ldax_tcthwm=0" ldax)

cbxp_test_null(extract.filter.null.ldax_id_INVALID                         ${EXE} extract -f "cvt.asvt.ascb.assb.ldax.ldax_id=INVALID" -i cvt.asvt.ascb.assb.ldax psa)

# =============================================================================
# Extract – Errors: Bad Usage
# =============================================================================

cbxp_test_fail(extract.error.no_args                ${EXE} extract)
cbxp_test_fail(extract.error.bad_flag_junk          ${EXE} extract --junk)
cbxp_test_fail(extract.error.bad_flag_junk_junk     ${EXE} extract --junk junk)
cbxp_test_fail(extract.error.unknown_flag_x         ${EXE} extract -x "unknown flag" cvt)
cbxp_test_fail(extract.error.include_no_block       ${EXE} extract -i cvt)
cbxp_test_fail(extract.error.include_repeat         ${EXE} extract -i -i cvt psa)
cbxp_test_fail(extract.error.filter_no_block        ${EXE} extract -f psa)
cbxp_test_fail(extract.error.filter_bad_field       ${EXE} extract -f psapsa=psa psa)
cbxp_test_fail(extract.error.bad_flag_F             ${EXE} extract -F ${SAMPLES}/ascb.bin psa)
cbxp_test_fail(extract.error.bad_flag_o             ${EXE} extract -o 1 psa)

# =============================================================================
# Extract – Errors: Unknown Control Block
# =============================================================================

cbxp_test_fail(extract.error.unknown_block          ${EXE} extract unknown)

# =============================================================================
# Extract – Errors: Bad Include Patterns
# =============================================================================

cbxp_test_fail(extract.error.include.comma_asvt_ascb_cvt   ${EXE} extract -i asvt,ascb cvt)
cbxp_test_fail(extract.error.include.comma_asvt_as_cvt     ${EXE} extract -i asvt,as cvt)
cbxp_test_fail(extract.error.include.asvt.ascb_psa         ${EXE} extract -i asvt.ascb psa)
cbxp_test_fail(extract.error.include.ascb_psa              ${EXE} extract -i ascb psa)
cbxp_test_fail(extract.error.include.ecvt_ascb             ${EXE} extract -i ecvt ascb)
cbxp_test_fail(extract.error.include.cvt.ecvt_cvt.ascb_psa ${EXE} extract -i cvt.ecvt -i cvt.ascb psa)
cbxp_test_fail(extract.error.include.cvt.asvt.ascb_ecvt    ${EXE} extract -i cvt.asvt.ascb -i ecvt psa)
cbxp_test_fail(extract.error.include.cvt_cvt               ${EXE} extract -i cvt cvt)
cbxp_test_fail(extract.error.include.ldax_psa              ${EXE} extract -i ldax psa)
cbxp_test_fail(extract.error.include.ldax_cvt              ${EXE} extract -i ldax cvt)
cbxp_test_fail(extract.error.include.ldax_ascb             ${EXE} extract -i ldax ascb)

# =============================================================================
# Extract – Errors: Bad Filters
# =============================================================================

cbxp_test_fail(extract.error.filter.assbjbns_lowercase_psa      ${EXE} extract -f "cvt.asvt.ascb.assb.assbjbns=*master*" psa)
cbxp_test_fail(extract.error.filter.assbjbns_lt_glob_psa        ${EXE} extract -f "cvt.asvt.ascb.assb.assbjbns<*master*" -i "**" psa)
cbxp_test_fail(extract.error.filter.psapsb_eq_PSA               ${EXE} extract -f psapsb=PSA psa)
cbxp_test_fail(extract.error.filter.assbasid_empty              ${EXE} extract -f assbasid= assb)
cbxp_test_fail(extract.error.filter.ascbasid_le_junk            ${EXE} extract -f "ascbasid<=junk" ascb)
cbxp_test_fail(extract.error.filter.comma_multi_filter          ${EXE} extract -f "psapsa=psa,cvt.asvt.ascb.ascbasid<2" cvt)
cbxp_test_fail(extract.error.filter.junk_psa                    ${EXE} extract -f junk psa)

# =============================================================================
# Format – Show Usage
# =============================================================================

cbxp_test(format.help_short           ${EXE} format -h)
cbxp_test(format.help_long            ${EXE} format --help)

# =============================================================================
# Format – Basic Usage
# =============================================================================

cbxp_test(format.ascb_file            ${EXE} format -F ${SAMPLES}/ascb.bin ascb)
cbxp_test(format.oucb_file            ${EXE} format -F ${SAMPLES}/oucb.bin oucb)

# format.cvt_pipe: pipe via shell — CTest runs commands directly, so we use
# cmake -E env + sh to emulate "cat file | cmd"
cbxp_test(format.cvt_pipe
    ${CMAKE_COMMAND} -E env sh -c "cat ${SAMPLES}/cvt.bin | ${EXE} format cvt")

# =============================================================================
# Format – Debug Mode
# =============================================================================

cbxp_test(format.debug_short          ${EXE} format -F ${SAMPLES}/ascb.bin -d ascb)
cbxp_test(format.debug_long           ${EXE} format -F ${SAMPLES}/ascb.bin --debug ascb)

# =============================================================================
# Format – Offset
# =============================================================================

cbxp_test(format.offset_hex           ${EXE} format -F ${SAMPLES}/ascb_offset_0x40.bin -o 0x40 ascb)
cbxp_test(format.offset_decimal       ${EXE} format -F ${SAMPLES}/ascb_offset_0x40.bin -o 64 ascb)
cbxp_test(format.offset_oucb_pipe_hex
    ${CMAKE_COMMAND} -E env sh -c "cat ${SAMPLES}/oucb_offset_0x03a8.bin | ${EXE} format -o 0x03a8 oucb")
cbxp_test(format.offset_oucb_pipe_decimal
    ${CMAKE_COMMAND} -E env sh -c "cat ${SAMPLES}/oucb_offset_0x03a8.bin | ${EXE} format -o 936 oucb")

# =============================================================================
# Format – Errors: Bad Usage
# =============================================================================

cbxp_test_fail(format.error.no_args               ${EXE} format)
cbxp_test_fail(format.error.bad_flag_junk         ${EXE} format --junk)
cbxp_test_fail(format.error.bad_flag_junk_junk    ${EXE} format --junk junk)
cbxp_test_fail(format.error.no_file_psa           ${EXE} format psa)
cbxp_test_fail(format.error.flag_i_not_supported  ${EXE} format -i cvt psa)
cbxp_test_fail(format.error.flag_f_not_supported  ${EXE} format -f psapsa=PSA psa)

# =============================================================================
# Format – Errors: Unknown Control Block
# =============================================================================

cbxp_test_fail(format.error.unknown_block         ${EXE} format -F ${SAMPLES}/ascb.bin -d unknown)

# =============================================================================
# Format – Errors: Bad Offset
# =============================================================================

cbxp_test_fail(format.error.offset_too_large      ${EXE} format -F ${SAMPLES}/ascb.bin -o 999999 ascb)
cbxp_test_fail(format.error.offset_negative       ${EXE} format -F ${SAMPLES}/ascb.bin -o -1 ascb)
cbxp_test_fail(format.error.offset_junk           ${EXE} format -F ${SAMPLES}/ascb.bin -o JUNK ascb)
cbxp_test_fail(format.error.offset_float          ${EXE} format -F ${SAMPLES}/ascb.bin -o 5.5 ascb)

# =============================================================================
# Format – Errors: Error Opening File
# =============================================================================

cbxp_test_fail(format.error.file_not_found        ${EXE} format -F ${SAMPLES}/notreal.bin psa)

# =============================================================================
# Format – Errors: No File/Pipe provided
# =============================================================================

cbxp_test_fail(format.error.no_file_or_pipe       ${EXE} format psa)

# =============================================================================
# Format – Errors: File and Pipe
# =============================================================================

cbxp_test_fail(format.error.file_and_pipe
    ${CMAKE_COMMAND} -E env sh -c "cat ${SAMPLES}/cvt.bin | ${EXE} format -F ${SAMPLES}/cvt.bin cvt")

# =============================================================================
# Format – Errors: Data Too Small
# =============================================================================

cbxp_test_fail(format.error.data_too_small        ${EXE} format -F ${SAMPLES}/ascb.bin psa)
