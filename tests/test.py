import unittest

from cbxp import CBXPError, CBXPFilter, CBXPFilterOperation, cbxp


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

    def test_cbxp_can_extract_oucb(self):
        cbdata = cbxp("oucb")
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

    def test_cbxp_can_extract_the_ascb_and_include_the_oucb(self):
        cbdata = cbxp("ascb", includes=["oucb"])
        self.assertIs(type(cbdata), list)
        for entry in cbdata:
            self.assertIs(type(entry), dict)
            self.assertIs(type(entry["ascboucb"]), dict)

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

    def test_cbxp_include_can_extract_psa_and_include_ecvt_asvt_and_cvt_asvt_ascb_oucb(
        self,
    ):
        cbdata = cbxp("psa", includes=["cvt.ecvt", "cvt.asvt.ascb.oucb"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["flccvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtecvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]["asvtenty"]), list)
        for entry in cbdata["flccvt"]["cvtasvt"]["asvtenty"]:
            self.assertIs(type(entry), dict)
            self.assertIs(type(entry["ascboucb"]), dict)

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
            self.assertIs(type(entry["ascboucb"]), dict)

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
            self.assertIs(type(entry["ascboucb"]), dict)

    # ============================================================================
    # Filters
    # ============================================================================
    def test_cbxp_can_use_filter(self):
        cbdata = cbxp(
            "psa",
            filters=[CBXPFilter("psapsa", CBXPFilterOperation.EQUAL, "PSA")],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_use_filter_with_wildcard_include(self):
        cbdata = cbxp(
            "psa",
            filters=[
                CBXPFilter(
                    "cvt.asvt.ascb.assb.assbjbns",
                    CBXPFilterOperation.EQUAL,
                    "*MASTER*",
                ),
            ],
            includes=["**"],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_use_filter_with_explicit_include(self):
        cbdata = cbxp(
            "psa",
            filters=[
                CBXPFilter(
                    "cvt.asvt.ascb.assb.assbjbns",
                    CBXPFilterOperation.EQUAL,
                    "*MASTER*",
                ),
            ],
            includes=["cvt.asvt.ascb.assb"],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_use_multiple_filters(self):
        cbdata = cbxp(
            "psa",
            filters=[
                CBXPFilter(
                    "cvt.asvt.ascb.assb.assbjbns",
                    CBXPFilterOperation.EQUAL,
                    "*MASTER*",
                ),
                CBXPFilter(
                    "cvt.asvt.ascb.ascbasid",
                    CBXPFilterOperation.GREATER_THAN,
                    0,
                ),
            ],
            includes=["cvt.asvt.ascb.assb"],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_use_wildcard_filter_with_string(self):
        cbdata = cbxp(
            "psa",
            filters=[
                CBXPFilter(
                    "cvt.asvt.ascb.assb.assbjbns",
                    CBXPFilterOperation.EQUAL,
                    "?MAS?ER?",
                ),
            ],
            includes=["cvt.asvt.ascb.assb"],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_use_int_filter_equal(self):
        cbdata = cbxp(
            "psa",
            filters=[
                CBXPFilter("cvt.asvt.ascb.ascbasid", CBXPFilterOperation.EQUAL, 1),
            ],
            includes=["cvt.asvt.ascb.assb"],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_use_int_filter_greater_than(self):
        cbdata = cbxp(
            "psa",
            filters=[
                CBXPFilter(
                    "cvt.asvt.ascb.ascbasid",
                    CBXPFilterOperation.GREATER_THAN,
                    0,
                ),
            ],
            includes=["cvt.asvt.ascb.assb"],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_use_int_filter_less_than(self):
        cbdata = cbxp(
            "psa",
            filters=[
                CBXPFilter(
                    "cvt.asvt.ascb.ascbasid",
                    CBXPFilterOperation.LESS_THAN,
                    2,
                ),
            ],
            includes=["cvt.asvt.ascb.assb"],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_use_int_filter_greater_than_or_equal(self):
        cbdata = cbxp(
            "psa",
            filters=[
                CBXPFilter(
                    "cvt.asvt.ascb.ascbasid",
                    CBXPFilterOperation.GREATER_THAN_OR_EQUAL,
                    1,
                ),
            ],
            includes=["cvt.asvt.ascb.assb"],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_use_int_filter_less_than_or_equal(self):
        cbdata = cbxp(
            "psa",
            filters=[
                CBXPFilter(
                    "cvt.asvt.ascb.ascbasid",
                    CBXPFilterOperation.LESS_THAN_OR_EQUAL,
                    2,
                ),
            ],
            includes=["cvt.asvt.ascb.assb"],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_use_int_filter_with_hex_field_equal(self):
        cbdata = cbxp(
            "cvt",
            filters=[CBXPFilter("cvtasmvt", CBXPFilterOperation.EQUAL, 2281701376)],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_use_hex_filter_with_equal(self):
        cbdata = cbxp(
            "cvt",
            filters=[CBXPFilter("cvtasmvt", CBXPFilterOperation.EQUAL, "0x88000000")],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_use_hex_filter_with_greater_than(self):
        cbdata = cbxp(
            "cvt",
            filters=[
                CBXPFilter("cvtasmvt", CBXPFilterOperation.GREATER_THAN, "0x87FFFFFF"),
            ],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_use_hex_filter_with_less_than(self):
        cbdata = cbxp(
            "cvt",
            filters=[
                CBXPFilter("cvtasmvt", CBXPFilterOperation.LESS_THAN, "0x88000001"),
            ],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_use_hex_filter_with_greater_than_or_equal(self):
        cbdata = cbxp(
            "cvt",
            filters=[
                CBXPFilter(
                    "cvtasmvt",
                    CBXPFilterOperation.GREATER_THAN_OR_EQUAL,
                    "0x87FFFFFF",
                ),
            ],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_can_use_hex_filter_with_less_than_or_equal(self):
        cbdata = cbxp(
            "cvt",
            filters=[
                CBXPFilter(
                    "cvtasmvt",
                    CBXPFilterOperation.LESS_THAN_OR_EQUAL,
                    "0x88000000",
                ),
            ],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_returns_none_if_no_filter_match(
        self,
    ):
        self.assertIsNone(
            cbxp(
                "psa",
                filters=[CBXPFilter("psapsa", CBXPFilterOperation.EQUAL, "PSB")],
            ),
        )

    def test_cbxp_returns_none_if_one_of_two_filters_fails(
        self,
    ):
        self.assertIsNone(
            cbxp(
                "ascb",
                includes=["assb"],
                filters=[
                    CBXPFilter(
                        "assb.assbjbns",
                        CBXPFilterOperation.EQUAL,
                        "*MASTER*",
                    ),
                    CBXPFilter(
                        "ascbasid",
                        CBXPFilterOperation.GREATER_THAN,
                        2,
                    ),
                ],
            ),
        )

    def test_cbxp_can_use_filter_oucbtrxn_from_oucb(self):
        cbdata = cbxp(
            "oucb",
            filters=[CBXPFilter("oucbtrxn", CBXPFilterOperation.EQUAL, "OMVS")],
        )
        self.assertIs(type(cbdata), list)
        for entry in cbdata:
            self.assertIs(type(entry), dict)
            self.assertEqual(entry["oucbtrxn"], "OMVS")

    def test_cbxp_can_use_filter_on_ascb_oucb_oucbtrxn_with_explicit_include_oucb(self):
        cbdata = cbxp(
            "ascb",
            filters=[
                CBXPFilter(
                    "oucb.oucbtrxn",
                    CBXPFilterOperation.EQUAL,
                    "OMVS",
                ),
            ],
            includes=["oucb"],
        )
        self.assertIs(type(cbdata), list)
        for entry in cbdata:
            self.assertIs(type(entry), dict)
            self.assertIs(type(entry["ascboucb"]), dict)
            self.assertEqual(entry["ascboucb"]["oucbtrxn"], "OMVS")

    def test_cbxp_can_use_null_filter_string(
        self,
    ):
        cbdata = cbxp(
            "assb",
            filters=[
                CBXPFilter(
                    "assbjbns",
                    CBXPFilterOperation.EQUAL,
                    "*MASTER*",
                ),
                CBXPFilter(
                    "assbjbni",
                    CBXPFilterOperation.EQUAL,
                    "",
                ),
            ],
        )
        self.assertIs(type(cbdata), list)

    def test_cbxp_can_use_empty_single_quotes_as_null_filter_string(
        self,
    ):
        cbdata = cbxp(
            "assb",
            filters=[
                CBXPFilter(
                    "assbjbns",
                    CBXPFilterOperation.EQUAL,
                    "*MASTER*",
                ),
                CBXPFilter(
                    "assbjbni",
                    CBXPFilterOperation.EQUAL,
                    "",
                ),
            ],
        )
        self.assertIs(type(cbdata), list)

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
                filters=[
                    CBXPFilter(
                        "cvt.asvt.ascb.assb.assbjbns",
                        CBXPFilterOperation.EQUAL,
                        "*MASTER*",
                    ),
                ],
            )
        self.assertEqual("A bad filter was provided", str(e.exception))

    def test_cbxp_raises_cbxp_error_if_non_equality_filter_used_with_string(
        self,
    ):
        with self.assertRaises(CBXPError) as e:
            cbxp(
                "psa",
                includes=["**"],
                filters=[
                    CBXPFilter(
                        "cvt.asvt.ascb.assb.assbjbns",
                        CBXPFilterOperation.LESS_THAN_OR_EQUAL,
                        "*MASTER*",
                    ),
                ],
            )
        self.assertEqual("A bad filter was provided", str(e.exception))

    def test_cbxp_raises_cbxp_error_if_filter_uses_unknown_key(
        self,
    ):
        with self.assertRaises(CBXPError) as e:
            cbxp(
                "psa",
                filters=["psapsb", CBXPFilterOperation.EQUAL, "PSA"],
            )
        self.assertEqual("A bad filter was provided", str(e.exception))

    def test_cbxp_raises_cbxp_error_if_filter_passes_null_value_for_non_string(
        self,
    ):
        with self.assertRaises(CBXPError) as e:
            cbxp(
                "assb",
                filters=["assbasid", CBXPFilterOperation.EQUAL, ""],
            )
        self.assertEqual("A bad filter was provided", str(e.exception))

    def test_cbxp_raises_cbxp_error_if_filter_uses_string_for_numeric_field(
        self,
    ):
        with self.assertRaises(CBXPError) as e:
            cbxp(
                "ascb",
                filters=["ascbasid", CBXPFilterOperation.LESS_THAN, "junk"],
            )
        self.assertEqual("A bad filter was provided", str(e.exception))

    def test_cbxp_raises_cbxp_error_if_filter_has_comma(
        self,
    ):
        with self.assertRaises(CBXPError) as e:
            cbxp(
                "psa",
                filters=["psapsa", CBXPFilterOperation.EQUAL, "PSA,PSB"],
            )
        self.assertEqual("Filters cannot contain commas", str(e.exception))


if __name__ == "__main__":
    unittest.main(verbosity=2, failfast=True, buffer=True)
