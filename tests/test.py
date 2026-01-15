import unittest

from cbxp import CBXPError, cbxp


class TestCBXP(unittest.TestCase):
    # ============================================================================
    # Basic Usage
    # ============================================================================
    def test_cbxp_can_extract_psa(self):
        cbdata = cbxp("psa")
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_extract_cvt(self):
        cbdata = cbxp("cvt")
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_extract_ecvt(self):
        cbdata = cbxp("ecvt")
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_extract_asvt(self):
        cbdata = cbxp("asvt")
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_extract_ascb(self):
        cbdata = cbxp("ascb")
        self.assertIs(type(cbdata), list)
        for entry in cbdata:
            self.assertIs(type(entry), dict)

    def test_cbxp_can_extract_assb(self):
        cbdata = cbxp("assb")
        self.assertIs(type(cbdata), list)
        for entry in cbdata:
            self.assertIs(type(entry), dict)

    # ============================================================================
    # Include Patterns
    # ============================================================================
    def test_cbxp_can_extract_the_psa_and_include_the_cvt(self):
        cbdata = cbxp("psa", includes=["cvt"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["flccvt"]), dict)

    def test_cbxp_can_extract_the_cvt_and_include_the_ecvt(self):
        cbdata = cbxp("cvt", includes=["ecvt"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["cvtecvt"]), dict)

    def test_cbxp_can_extract_the_cvt_and_include_the_asvt(self):
        cbdata = cbxp("cvt", includes=["asvt"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["cvtasvt"]), dict)
        self.assertIs(type(cbdata["cvtasvt"]["asvtenty"]), list)
        for entry in cbdata["cvtasvt"]["asvtenty"]:
            self.assertIs(type(entry), str)

    def test_cbxp_can_extract_the_asvt_and_include_the_ascb(self):
        cbdata = cbxp("asvt", includes=["ascb"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["asvtenty"]), list)
        for entry in cbdata["asvtenty"]:
            self.assertIs(type(entry), dict)

    def test_cbxp_can_extract_the_ascb_and_include_the_assb(self):
        cbdata = cbxp("ascb", includes=["assb"])
        self.assertIs(type(cbdata), list)
        for entry in cbdata:
            self.assertIs(type(entry), dict)
            self.assertIs(type(entry["ascbassb"]), dict)

    def test_cbxp_can_extract_the_psa_and_include_the_cvt_ecvt(self):
        cbdata = cbxp("psa", includes=["cvt.ecvt"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["flccvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtecvt"]), dict)

    def test_cbxp_can_extract_the_psa_and_include_the_cvt_ecvt_ascb(self):
        cbdata = cbxp("psa", includes=["cvt.asvt.ascb"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["flccvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]["asvtenty"]), list)
        for entry in cbdata["flccvt"]["cvtasvt"]["asvtenty"]:
            self.assertIs(type(entry), dict)

    def test_cbxp_can_extract_the_cvt_and_include_the_asvt_ascb(self):
        cbdata = cbxp("cvt", includes=["asvt.ascb"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["cvtasvt"]), dict)
        self.assertIs(type(cbdata["cvtasvt"]["asvtenty"]), list)
        for entry in cbdata["cvtasvt"]["asvtenty"]:
            self.assertIs(type(entry), dict)

    def test_cbxp_include_can_extract_cvt_and_include_ecvt_and_asvt(self):
        cbdata = cbxp("cvt", includes=["ecvt", "asvt"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["cvtecvt"]), dict)
        self.assertIs(type(cbdata["cvtasvt"]), dict)
        self.assertIs(type(cbdata["cvtasvt"]["asvtenty"]), list)
        for entry in cbdata["cvtasvt"]["asvtenty"]:
            self.assertIs(type(entry), str)

    def test_cbxp_include_can_extract_psa_and_include_ecvt_asvt_and_cvt_asvt_ascb(
        self,
    ):
        cbdata = cbxp("psa", includes=["cvt.ecvt", "cvt.asvt.ascb"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["flccvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtecvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]["asvtenty"]), list)
        for entry in cbdata["flccvt"]["cvtasvt"]["asvtenty"]:
            self.assertIs(type(entry), dict)

    def test_cbxp_include_can_extract_psa_and_include_ecvt_asvt_and_cvt_asvt_ascb_assb(
        self,
    ):
        cbdata = cbxp("psa", includes=["cvt.ecvt", "cvt.asvt.ascb.assb"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["flccvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtecvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]["asvtenty"]), list)
        for entry in cbdata["flccvt"]["cvtasvt"]["asvtenty"]:
            self.assertIs(type(entry), dict)
            self.assertIs(type(entry["ascbassb"]), dict)

    def test_cbxp_can_extract_psa_and_include_cvt_recursive_wildcard(self):
        cbdata = cbxp("psa", includes=["cvt.**"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["flccvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtecvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]["asvtenty"]), list)
        for entry in cbdata["flccvt"]["cvtasvt"]["asvtenty"]:
            self.assertIs(type(entry), dict)
            self.assertIs(type(entry["ascbassb"]), dict)

    def test_cbxp_can_extract_psa_and_include_cvt_wildcard(self):
        cbdata = cbxp("psa", includes=["cvt.*"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["flccvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtecvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]["asvtenty"]), list)
        for entry in cbdata["flccvt"]["cvtasvt"]["asvtenty"]:
            self.assertIs(type(entry), str)

    def test_cbxp_can_extract_cvt_and_include_wildcard_and_asvt_wildcard(self):
        cbdata = cbxp("cvt", includes=["*", "asvt.*"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["cvtecvt"]), dict)
        self.assertIs(type(cbdata["cvtasvt"]), dict)
        self.assertIs(type(cbdata["cvtasvt"]["asvtenty"]), list)
        for entry in cbdata["cvtasvt"]["asvtenty"]:
            self.assertIs(type(entry), dict)

    def test_cbxp_can_extract_cvt_and_include_wildcard_and_asvt_recursive_wildcard(
        self,
    ):
        cbdata = cbxp("cvt", includes=["*", "asvt.**"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["cvtecvt"]), dict)
        self.assertIs(type(cbdata["cvtasvt"]), dict)
        self.assertIs(type(cbdata["cvtasvt"]["asvtenty"]), list)
        for entry in cbdata["cvtasvt"]["asvtenty"]:
            self.assertIs(type(entry), dict)
            self.assertIs(type(entry["ascbassb"]), dict)

    # ============================================================================
    # Filters
    # ============================================================================
    def test_cbxp_can_use_basic_filter(self):
        cbdata = cbxp("psa", control_block_filters=["psapsa=PSA"])
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_use_include_filter_with_wildcard_include(self):
        cbdata = cbxp(
            "psa",
            control_block_filters=["cvt.asvt.ascb.assb.assbjbns=*MASTER*"],
            includes=["**"],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_use_include_filter_with_explicit_include(self):
        cbdata = cbxp(
            "psa",
            control_block_filters=["cvt.asvt.ascb.assb.assbjbns=*MASTER*"],
            includes=["cvt.asvt.ascb.assb"],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_use_multiple_include_filters(self):
        cbdata = cbxp(
            "psa",
            control_block_filters=[
                "cvt.asvt.ascb.assb.assbjbns=*MASTER*",
                "cvt.asvt.ascb.ascbasid>0",
            ],
            includes=["cvt.asvt.ascb.assb"],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_use_wildcard_filter_with_string(self):
        cbdata = cbxp(
            "psa",
            control_block_filters=["cvt.asvt.ascb.assb.assbjbns=?MAS?ER?"],
            includes=["cvt.asvt.ascb.assb"],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_use_int_filter_equals(self):
        cbdata = cbxp(
            "psa",
            control_block_filters=["cvt.asvt.ascb.ascbasid=1"],
            includes=["cvt.asvt.ascb.assb"],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_use_int_filter_greater_than(self):
        cbdata = cbxp(
            "psa",
            control_block_filters=["cvt.asvt.ascb.ascbasid>0"],
            includes=["cvt.asvt.ascb.assb"],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_use_int_filter_less_than(self):
        cbdata = cbxp(
            "psa",
            control_block_filters=["cvt.asvt.ascb.ascbasid<2"],
            includes=["cvt.asvt.ascb.assb"],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_use_int_filter_greater_than_or_equals(self):
        cbdata = cbxp(
            "psa",
            control_block_filters=["cvt.asvt.ascb.ascbasid<=2"],
            includes=["cvt.asvt.ascb.assb"],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_use_int_filter_less_than_or_equals(self):
        cbdata = cbxp(
            "psa",
            control_block_filters=["cvt.asvt.ascb.ascbasid>=1"],
            includes=["cvt.asvt.ascb.assb"],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_decimal_filter_for_hex_field_with_equals(self):
        cbdata = cbxp(
            "cvt",
            control_block_filters=["cvtasmvt=2281701376"],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_use_hex_filter_with_greater_than(self):
        cbdata = cbxp(
            "cvt",
            control_block_filters=["cvtasmvt>0x87FFFFFF"],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_use_hex_filter_with_less_than(self):
        cbdata = cbxp(
            "cvt",
            control_block_filters=["cvtasmvt<0x88000001"],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_use_hex_filter_with_greater_than_or_equals(self):
        cbdata = cbxp(
            "cvt",
            control_block_filters=["cvtasmvt>=0x87FFFFFF"],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_use_hex_filter_with_less_than_or_equals(self):
        cbdata = cbxp(
            "cvt",
            control_block_filters=["cvtasmvt<=0x88000000"],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_returns_none_if_no_filter_match(
        self,
    ):
        self.assertIsNone(cbxp("psa", control_block_filters=["psapsa=PSB"]))

    def test_cbxp_returns_none_if_one_of_two_filters_fails(
        self,
    ):
        self.assertIsNone(
            cbxp(
                "psa",
                includes=["**"],
                control_block_filters=[
                    "cvt.asvt.ascb.assb.assbjbns=*MASTER*",
                    "cvt.asvt.ascb.ascbasid=2",
                ],
            )
        )

    # ============================================================================
    # Debug Mode
    # ============================================================================
    def test_cbxp_can_run_in_debug_mode(self):
        cbdata = cbxp("psa", debug=True)
        self.assertIs(type(cbdata), dict)

    # ============================================================================
    # Errors: Unknown Control Block
    # ============================================================================
    def test_cbxp_raises_cbxp_error_when_unknown_control_block_is_provided(self):
        with self.assertRaises(CBXPError) as e:
            cbxp("unknown")
        self.assertEqual(
            "Unknown control block 'unknown' was specified.",
            str(e.exception),
        )

    # ============================================================================
    # Errors: Bad Include Patterns
    # ============================================================================
    def test_cbxp_raises_cbxp_error_if_asvt_ascb_is_included_with_the_psa(
        self,
    ):
        with self.assertRaises(CBXPError) as e:
            cbxp("psa", includes=["asvt.ascb"])
        self.assertEqual("A bad include pattern was provided", str(e.exception))

    def test_cbxp_raises_cbxp_error_if_ascb_is_included_with_the_psa(self):
        with self.assertRaises(CBXPError) as e:
            cbxp("psa", includes=["ascb"])
        self.assertEqual("A bad include pattern was provided", str(e.exception))

    def test_cbxp_raises_cbxp_error_if_ecvt_is_included_with_ascb(self):
        with self.assertRaises(CBXPError) as e:
            cbxp("ascb", includes=["ecvt"])
        self.assertEqual("A bad include pattern was provided", str(e.exception))

    def test_cbxp_raises_cbxp_error_if_ect_ecvt_and_cvt_ascb_is_included_with_the_psa(
        self,
    ):
        with self.assertRaises(CBXPError) as e:
            cbxp("psa", includes=["cvt.ecvt", "cvt.ascb"])
        self.assertEqual("A bad include pattern was provided", str(e.exception))

    def test_cbxp_raises_cbxp_error_if_ecvt_and_cvt_asvt_ascb_is_included_with_the_psa(
        self,
    ):
        with self.assertRaises(CBXPError) as e:
            cbxp("psa", includes=["ecvt", "cvt.asvt.ascb"])
        self.assertEqual("A bad include pattern was provided", str(e.exception))

    def test_cbxp_raises_cbxp_error_if_cvt_is_included_with_the_cvt(self):
        with self.assertRaises(CBXPError) as e:
            cbxp("cvt", includes=["cvt"])
        self.assertEqual("A bad include pattern was provided", str(e.exception))

    def test_cbxp_raises_cbxp_error_when_pattern_cannot_contain_comma_1(self):
        with self.assertRaises(CBXPError) as e:
            cbxp("cvt", includes=["asvt,ascb"])
        self.assertEqual("Include patterns cannot contain commas", str(e.exception))

    def test_cbxp_raises_cbxp_error_when_pattern_cannot_contain_comma_2(self):
        with self.assertRaises(CBXPError) as e:
            cbxp("cvt", includes=["asvt,as"])
        self.assertEqual("Include patterns cannot contain commas", str(e.exception))

    # ============================================================================
    # Errors: Bad Filters
    # ============================================================================
    def test_cbxp_raises_cbxp_error_if_filter_uses_non_included_control_block(
        self,
    ):
        with self.assertRaises(CBXPError) as e:
            cbxp(
                "psa",
                control_block_filters=["cvt.asvt.ascb.assb.assbjbns=*MASTER*"],
            )
        self.assertEqual("A bad filter was provided", str(e.exception))

    def test_cbxp_raises_cbxp_error_if_non_equality_filter_used_with_string(
        self,
    ):
        with self.assertRaises(CBXPError) as e:
            cbxp(
                "psa",
                includes=["**"],
                control_block_filters=["cvt.asvt.ascb.assb.assbjbns<*master*"],
            )
        self.assertEqual("A bad filter was provided", str(e.exception))

    def test_cbxp_raises_cbxp_error_if_filter_uses_unknown_key(
        self,
    ):
        with self.assertRaises(CBXPError) as e:
            cbxp("psa", control_block_filters=["psapsb=PSA"])
        self.assertEqual("A bad filter was provided", str(e.exception))

    def test_cbxp_raises_cbxp_error_if_filter_passes_null_value(
        self,
    ):
        with self.assertRaises(CBXPError) as e:
            cbxp("psa", control_block_filters=["psapsa="])
        self.assertEqual("A bad filter was provided", str(e.exception))

    def test_cbxp_raises_cbxp_error_if_filter_has_comma(
        self,
    ):
        with self.assertRaises(CBXPError) as e:
            cbxp("psa", control_block_filters=["psapsa=PSA,cvt.cvtasmvt<88000001"])
        self.assertEqual("Filters cannot contain commas", str(e.exception))


if __name__ == "__main__":
    unittest.main(verbosity=2, failfast=True, buffer=True)
