import unittest
from pathlib import Path

from cbxp import CBXPError, CBXPFilter, CBXPFilterOperation, cbxp


class TestCBXP(unittest.TestCase):
    SAMPLE_DIR = Path(__file__).resolve().parent / "samples"

    # ============================================================================
    # Utility Functions
    # ============================================================================

    @staticmethod
    def read_sample(filename: str) -> bytes:
        return (TestCBXP.SAMPLE_DIR / filename).read_bytes()

    @staticmethod
    def get_cvtasmvt_values() -> tuple[int, str]:
        cbdata = cbxp.extract("cvt")
        cvtasmvt_hex = cbdata["cvtasmvt"]
        return int(cvtasmvt_hex, 16), cvtasmvt_hex

    # ============================================================================
    # Extract -- Basic Usage
    # ============================================================================
    def test_cbxp_extract_psa(self):
        cbdata = cbxp.extract("psa")
        self.assertIs(type(cbdata), dict)

    def test_cbxp_extract_cvt(self):
        cbdata = cbxp.extract("cvt")
        self.assertIs(type(cbdata), dict)

    def test_cbxp_extract_ecvt(self):
        cbdata = cbxp.extract("ecvt")
        self.assertIs(type(cbdata), dict)

    def test_cbxp_extract_asvt(self):
        cbdata = cbxp.extract("asvt")
        self.assertIs(type(cbdata), dict)

    def test_cbxp_extract_ascb(self):
        cbdata = cbxp.extract("ascb")
        self.assertIs(type(cbdata), list)
        for entry in cbdata:
            self.assertIs(type(entry), dict)

    def test_cbxp_extract_assb(self):
        cbdata = cbxp.extract("assb")
        self.assertIs(type(cbdata), list)
        for entry in cbdata:
            self.assertIs(type(entry), dict)

    def test_cbxp_extract_oucb(self):
        cbdata = cbxp.extract("oucb")
        self.assertIs(type(cbdata), list)
        for entry in cbdata:
            self.assertIs(type(entry), dict)

    # ============================================================================
    # Extract -- Debug Mode
    # ============================================================================
    def test_cbxp_extract_runs_in_debug_mode(self):
        cbdata = cbxp.extract("psa", debug=True)
        self.assertIs(type(cbdata), dict)

    # ============================================================================
    # Extract -- Include Patterns
    # ============================================================================
    def test_cbxp_extract_psa_and_include_cvt(self):
        cbdata = cbxp.extract("psa", includes=["cvt"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["flccvt"]), dict)

    def test_cbxp_extract_cvt_and_include_ecvt(self):
        cbdata = cbxp.extract("cvt", includes=["ecvt"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["cvtecvt"]), dict)

    def test_cbxp_extract_cvt_and_include_asvt(self):
        cbdata = cbxp.extract("cvt", includes=["asvt"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["cvtasvt"]), dict)
        self.assertIs(type(cbdata["cvtasvt"]["asvtenty"]), list)
        for entry in cbdata["cvtasvt"]["asvtenty"]:
            self.assertIs(type(entry), str)

    def test_cbxp_extract_asvt_and_include_ascb(self):
        cbdata = cbxp.extract("asvt", includes=["ascb"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["asvtenty"]), list)
        for entry in cbdata["asvtenty"]:
            self.assertIs(type(entry), dict)

    def test_cbxp_extract_ascb_and_include_assb(self):
        cbdata = cbxp.extract("ascb", includes=["assb"])
        self.assertIs(type(cbdata), list)
        for entry in cbdata:
            self.assertIs(type(entry), dict)
            self.assertIs(type(entry["ascbassb"]), dict)

    def test_cbxp_extract_ascb_and_include_oucb(self):
        cbdata = cbxp.extract("ascb", includes=["oucb"])
        self.assertIs(type(cbdata), list)
        for entry in cbdata:
            self.assertIs(type(entry), dict)
            self.assertIs(type(entry["ascboucb"]), dict)

    def test_cbxp_extract_psa_and_include_cvt_ecvt(self):
        cbdata = cbxp.extract("psa", includes=["cvt.ecvt"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["flccvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtecvt"]), dict)

    def test_cbxp_extract_psa_and_include_cvt_ecvt_ascb(self):
        cbdata = cbxp.extract("psa", includes=["cvt.asvt.ascb"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["flccvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]["asvtenty"]), list)
        for entry in cbdata["flccvt"]["cvtasvt"]["asvtenty"]:
            self.assertIs(type(entry), dict)

    def test_cbxp_extract_cvt_and_include_asvt_ascb(self):
        cbdata = cbxp.extract("cvt", includes=["asvt.ascb"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["cvtasvt"]), dict)
        self.assertIs(type(cbdata["cvtasvt"]["asvtenty"]), list)
        for entry in cbdata["cvtasvt"]["asvtenty"]:
            self.assertIs(type(entry), dict)

    def test_cbxp_include_extract_cvt_and_include_ecvt_and_asvt(self):
        cbdata = cbxp.extract("cvt", includes=["ecvt", "asvt"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["cvtecvt"]), dict)
        self.assertIs(type(cbdata["cvtasvt"]), dict)
        self.assertIs(type(cbdata["cvtasvt"]["asvtenty"]), list)
        for entry in cbdata["cvtasvt"]["asvtenty"]:
            self.assertIs(type(entry), str)

    def test_cbxp_include_extract_psa_and_include_ecvt_asvt_and_cvt_asvt_ascb(
        self,
    ):
        cbdata = cbxp.extract("psa", includes=["cvt.ecvt", "cvt.asvt.ascb"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["flccvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtecvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]["asvtenty"]), list)
        for entry in cbdata["flccvt"]["cvtasvt"]["asvtenty"]:
            self.assertIs(type(entry), dict)

    def test_cbxp_include_extract_psa_and_include_ecvt_asvt_and_cvt_asvt_ascb_assb(
        self,
    ):
        cbdata = cbxp.extract("psa", includes=["cvt.ecvt", "cvt.asvt.ascb.assb"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["flccvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtecvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]["asvtenty"]), list)
        for entry in cbdata["flccvt"]["cvtasvt"]["asvtenty"]:
            self.assertIs(type(entry), dict)
            self.assertIs(type(entry["ascbassb"]), dict)

    def test_cbxp_include_extract_psa_and_include_ecvt_asvt_and_cvt_asvt_ascb_oucb(
        self,
    ):
        cbdata = cbxp.extract("psa", includes=["cvt.ecvt", "cvt.asvt.ascb.oucb"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["flccvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtecvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]["asvtenty"]), list)
        for entry in cbdata["flccvt"]["cvtasvt"]["asvtenty"]:
            self.assertIs(type(entry), dict)
            self.assertIs(type(entry["ascboucb"]), dict)

    def test_cbxp_extract_psa_and_include_cvt_recursive_wildcard(self):
        cbdata = cbxp.extract("psa", includes=["cvt.**"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["flccvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtecvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]["asvtenty"]), list)
        for entry in cbdata["flccvt"]["cvtasvt"]["asvtenty"]:
            self.assertIs(type(entry), dict)
            self.assertIs(type(entry["ascbassb"]), dict)
            self.assertIs(type(entry["ascboucb"]), dict)

    def test_cbxp_extract_psa_and_include_cvt_wildcard(self):
        cbdata = cbxp.extract("psa", includes=["cvt.*"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["flccvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtecvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]["asvtenty"]), list)
        for entry in cbdata["flccvt"]["cvtasvt"]["asvtenty"]:
            self.assertIs(type(entry), str)

    def test_cbxp_extract_cvt_and_include_wildcard_and_asvt_wildcard(self):
        cbdata = cbxp.extract("cvt", includes=["*", "asvt.*"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["cvtecvt"]), dict)
        self.assertIs(type(cbdata["cvtasvt"]), dict)
        self.assertIs(type(cbdata["cvtasvt"]["asvtenty"]), list)
        for entry in cbdata["cvtasvt"]["asvtenty"]:
            self.assertIs(type(entry), dict)

    def test_cbxp_extract_cvt_and_include_wildcard_and_asvt_recursive_wildcard(
        self,
    ):
        cbdata = cbxp.extract("cvt", includes=["*", "asvt.**"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["cvtecvt"]), dict)
        self.assertIs(type(cbdata["cvtasvt"]), dict)
        self.assertIs(type(cbdata["cvtasvt"]["asvtenty"]), list)
        for entry in cbdata["cvtasvt"]["asvtenty"]:
            self.assertIs(type(entry), dict)
            self.assertIs(type(entry["ascbassb"]), dict)
            self.assertIs(type(entry["ascboucb"]), dict)

    # ============================================================================
    # Extract -- Filters
    # ============================================================================
    def test_cbxp_extract_psa_filter_eyecatcher(self):
        cbdata = cbxp.extract(
            "psa",
            filters=[CBXPFilter("psapsa", CBXPFilterOperation.EQUAL, "PSA")],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_extract_psa_filter_wildcard_include(self):
        cbdata = cbxp.extract(
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

    def test_cbxp_extract_filter_with_explicit_include(self):
        cbdata = cbxp.extract(
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

    def test_cbxp_extract_multiple_filters(self):
        cbdata = cbxp.extract(
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

    def test_cbxp_extract_wildcard_filter_with_string(self):
        cbdata = cbxp.extract(
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

    def test_cbxp_extract_int_filter_equal(self):
        cbdata = cbxp.extract(
            "psa",
            filters=[
                CBXPFilter("cvt.asvt.ascb.ascbasid", CBXPFilterOperation.EQUAL, 1),
            ],
            includes=["cvt.asvt.ascb.assb"],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_extract_int_filter_greater_than(self):
        cbdata = cbxp.extract(
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

    def test_cbxp_extract_int_filter_less_than(self):
        cbdata = cbxp.extract(
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

    def test_cbxp_extract_int_filter_greater_than_or_equal(self):
        cbdata = cbxp.extract(
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

    def test_cbxp_extract_int_filter_less_than_or_equal(self):
        cbdata = cbxp.extract(
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

    def test_cbxp_extract_int_filter_with_hex_field_equal(self):
        cvtasmvt_int, _ = self.get_cvtasmvt_values()
        cbdata = cbxp.extract(
            "cvt",
            filters=[CBXPFilter("cvtasmvt", CBXPFilterOperation.EQUAL, cvtasmvt_int)],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_extract_hex_filter_with_equal(self):
        _, cvtasmvt_hex = self.get_cvtasmvt_values()
        cbdata = cbxp.extract(
            "cvt",
            filters=[CBXPFilter("cvtasmvt", CBXPFilterOperation.EQUAL, cvtasmvt_hex)],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_extract_hex_filter_with_greater_than(self):
        cvtasmvt_int, _ = self.get_cvtasmvt_values()
        cbdata = cbxp.extract(
            "cvt",
            filters=[
                CBXPFilter(
                    "cvtasmvt",
                    CBXPFilterOperation.GREATER_THAN,
                    hex(cvtasmvt_int - 1),
                ),
            ],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_extract_hex_filter_with_less_than(self):
        cvtasmvt_int, _ = self.get_cvtasmvt_values()
        cbdata = cbxp.extract(
            "cvt",
            filters=[
                CBXPFilter(
                    "cvtasmvt",
                    CBXPFilterOperation.LESS_THAN,
                    hex(cvtasmvt_int + 1),
                ),
            ],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_extract_hex_filter_with_greater_than_or_equal(self):
        cvtasmvt_int, _ = self.get_cvtasmvt_values()
        cbdata = cbxp.extract(
            "cvt",
            filters=[
                CBXPFilter(
                    "cvtasmvt",
                    CBXPFilterOperation.GREATER_THAN_OR_EQUAL,
                    hex(cvtasmvt_int),
                ),
            ],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_extract_hex_filter_with_less_than_or_equal(self):
        cvtasmvt_int, _ = self.get_cvtasmvt_values()
        cbdata = cbxp.extract(
            "cvt",
            filters=[
                CBXPFilter(
                    "cvtasmvt",
                    CBXPFilterOperation.LESS_THAN_OR_EQUAL,
                    hex(cvtasmvt_int),
                ),
            ],
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_extract_returns_none_if_no_filter_match(
        self,
    ):
        self.assertIsNone(
            cbxp.extract(
                "psa",
                filters=[CBXPFilter("psapsa", CBXPFilterOperation.EQUAL, "PSB")],
            ),
        )

    def test_cbxp_extract_returns_none_if_one_of_two_filters_fails(
        self,
    ):
        self.assertIsNone(
            cbxp.extract(
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

    def test_cbxp_extract_filter_oucbtrxn_from_oucb(self):
        cbdata = cbxp.extract(
            "oucb",
            filters=[CBXPFilter("oucbtrxn", CBXPFilterOperation.EQUAL, "OMVS")],
        )
        self.assertIs(type(cbdata), list)
        for entry in cbdata:
            self.assertIs(type(entry), dict)
            self.assertEqual(entry["oucbtrxn"], "OMVS")

    def test_cbxp_extract_filter_on_ascb_oucb_oucbtrxn_with_explicit_include_oucb(self):
        cbdata = cbxp.extract(
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

    def test_cbxp_extract_null_filter_string(
        self,
    ):
        cbdata = cbxp.extract(
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
    # Extract -- Testing Errors: Unknown Control Block
    # ============================================================================
    def test_cbxp_extract_raises_cbxp_error_if_unknown_control_block_is_provided(self):
        with self.assertRaises(CBXPError) as e:
            cbxp.extract("unknown")
        self.assertEqual(
            "Unknown control block: unknown",
            str(e.exception),
        )

    # ============================================================================
    # Extract -- Testing Errors: Bad Include Patterns
    # ============================================================================
    def test_cbxp_extract_raises_cbxp_error_if_asvt_ascb_is_included_with_the_psa(
        self,
    ):
        with self.assertRaises(CBXPError) as e:
            cbxp.extract("psa", includes=["asvt.ascb"])
        self.assertEqual("A bad include pattern was provided", str(e.exception))

    def test_cbxp_extract_raises_cbxp_error_if_ascb_is_included_with_the_psa(self):
        with self.assertRaises(CBXPError) as e:
            cbxp.extract("psa", includes=["ascb"])
        self.assertEqual("A bad include pattern was provided", str(e.exception))

    def test_cbxp_extract_raises_cbxp_error_if_ecvt_is_included_with_ascb(self):
        with self.assertRaises(CBXPError) as e:
            cbxp.extract("ascb", includes=["ecvt"])
        self.assertEqual("A bad include pattern was provided", str(e.exception))

    def test_cbxp_extract_raises_cbxp_error_if_cvt_ascb_is_included_with_the_psa(
        self,
    ):
        # cvt.ecvt is also included but is not error source
        with self.assertRaises(CBXPError) as e:
            cbxp.extract("psa", includes=["cvt.ecvt", "cvt.ascb"])
        self.assertEqual("A bad include pattern was provided", str(e.exception))

    def test_cbxp_extract_raises_cbxp_error_if_ecvt_is_included_with_the_psa(
        self,
    ):
        # cvt.asvt.ascb is also included but is not error source
        with self.assertRaises(CBXPError) as e:
            cbxp.extract("psa", includes=["ecvt", "cvt.asvt.ascb"])
        self.assertEqual("A bad include pattern was provided", str(e.exception))

    def test_cbxp_extract_raises_cbxp_error_if_cvt_is_included_with_the_cvt(self):
        with self.assertRaises(CBXPError) as e:
            cbxp.extract("cvt", includes=["cvt"])
        self.assertEqual("A bad include pattern was provided", str(e.exception))

    def test_cbxp_extract_raises_cbxp_error_when_pattern_cannot_contain_comma_1(self):
        with self.assertRaises(CBXPError) as e:
            cbxp.extract("cvt", includes=["asvt,ascb"])
        self.assertEqual("Include patterns cannot contain commas", str(e.exception))

    def test_cbxp_extract_raises_cbxp_error_when_pattern_cannot_contain_comma_2(self):
        with self.assertRaises(CBXPError) as e:
            cbxp.extract("cvt", includes=["asvt,as"])
        self.assertEqual("Include patterns cannot contain commas", str(e.exception))

    # ============================================================================
    # Extract -- Testing Errors: Bad Filters
    # ============================================================================
    def test_cbxp_extract_raises_cbxp_error_if_filter_uses_non_included_control_block(
        self,
    ):
        with self.assertRaises(CBXPError) as e:
            cbxp.extract(
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

    def test_cbxp_extract_raises_cbxp_error_if_non_equality_filter_used_with_string(
        self,
    ):
        with self.assertRaises(CBXPError) as e:
            cbxp.extract(
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

    def test_cbxp_extract_raises_cbxp_error_if_filter_uses_unknown_key(
        self,
    ):
        with self.assertRaises(CBXPError) as e:
            cbxp.extract(
                "psa",
                filters=["psapsb", CBXPFilterOperation.EQUAL, "PSA"],
            )
        self.assertEqual("A bad filter was provided", str(e.exception))

    def test_cbxp_extract_raises_cbxp_error_if_filter_passes_null_value_for_non_string(
        self,
    ):
        with self.assertRaises(CBXPError) as e:
            cbxp.extract(
                "assb",
                filters=["assbasid", CBXPFilterOperation.EQUAL, ""],
            )
        self.assertEqual("A bad filter was provided", str(e.exception))

    def test_cbxp_extract_raises_cbxp_error_if_filter_uses_string_for_numeric_field(
        self,
    ):
        with self.assertRaises(CBXPError) as e:
            cbxp.extract(
                "ascb",
                filters=["ascbasid", CBXPFilterOperation.LESS_THAN, "junk"],
            )
        self.assertEqual("A bad filter was provided", str(e.exception))

    def test_cbxp_extract_raises_cbxp_error_if_no_operation_provided(
        self,
    ):
        with self.assertRaises(CBXPError) as e:
            cbxp.extract(
                "psa",
                filters=["junk", None, ""],
            )
        self.assertEqual("A bad filter was provided", str(e.exception))

    def test_cbxp_extract_raises_cbxp_error_if_filter_has_comma(
        self,
    ):
        with self.assertRaises(CBXPError) as e:
            cbxp.extract(
                "psa",
                filters=["psapsa", CBXPFilterOperation.EQUAL, "PSA,PSB"],
            )
        self.assertEqual("Filters cannot contain commas", str(e.exception))

    # ============================================================================
    # Format -- Basic Usage
    # ============================================================================
    def test_cbxp_format_ascb(self):
        cbdata = cbxp.format(
            "ascb",
            data=self.read_sample("ascb.bin"),
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_format_cvt(self):
        cbdata = cbxp.format(
            "cvt",
            data=self.read_sample("cvt.bin"),
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_format_oucb(self):
        cbdata = cbxp.format(
            "oucb",
            data=self.read_sample("oucb.bin"),
        )
        self.assertIs(type(cbdata), dict)

    # ============================================================================
    # Format -- Debug Mode
    # ============================================================================
    def test_cbxp_format_runs_in_debug_mode(self):
        cbdata = cbxp.format("ascb", data=self.read_sample("ascb.bin"), debug=True)
        self.assertIs(type(cbdata), dict)

    # ============================================================================
    # Format -- Offset
    # ============================================================================

    def test_cbxp_format_ascb_with_hex_offset(self):
        cbdata = cbxp.format(
            "ascb",
            data=self.read_sample("ascboffset40.bin"),
            offset=0x40,
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_format_ascb_with_decimal_offset(self):
        cbdata = cbxp.format(
            "ascb",
            data=self.read_sample("ascboffset40.bin"),
            offset=64,
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_format_oucb_with_hex_offset(self):
        cbdata = cbxp.format(
            "oucb",
            data=self.read_sample("oucboffset3A8.bin"),
            offset=0x3A8,
        )
        self.assertIs(type(cbdata), dict)

    def test_cbxp_format_oucb_with_decimal_offset(self):
        cbdata = cbxp.format(
            "oucb",
            data=self.read_sample("oucboffset3A8.bin"),
            offset=936,
        )
        self.assertIs(type(cbdata), dict)

    # ============================================================================
    # Format -- Testing Errors: Unknown Control Block
    # ============================================================================
    def test_cbxp_format_raises_cbxp_error_if_unknown_control_block_is_provided(self):
        with self.assertRaises(CBXPError) as e:
            cbxp.format("unknown", data=self.read_sample("ascb.bin"))
        self.assertEqual(
            "Unknown control block: unknown",
            str(e.exception),
        )

    # ============================================================================
    # Format -- Testing Errors: Bad Offset
    # ============================================================================

    def test_cbxp_format_raises_cbxp_error_if_offset_is_too_large(self):
        with self.assertRaises(CBXPError) as e:
            cbxp.format(
                "ascb",
                data=self.read_sample("ascb.bin"),
                offset=999999,
            )
        self.assertEqual(
            "Offset is too large for data provided",
            str(e.exception),
        )

    def test_cbxp_format_raises_cbxp_error_if_offset_is_negative(self):
        with self.assertRaises(CBXPError) as e:
            cbxp.format(
                "ascb",
                data=self.read_sample("ascb.bin"),
                offset=-1,
            )
        self.assertEqual(
            "Offset must be positive integer",
            str(e.exception),
        )

    def test_cbxp_format_raises_cbxp_error_if_offset_is_alpha(self):
        with self.assertRaises(CBXPError) as e:
            cbxp.format(
                "ascb",
                data=self.read_sample("ascb.bin"),
                offset="JUNK",
            )
        self.assertEqual(
            "Offset must be positive integer",
            str(e.exception),
        )

    def test_cbxp_format_raises_cbxp_error_if_offset_is_float(self):
        with self.assertRaises(CBXPError) as e:
            cbxp.format(
                "ascb",
                data=self.read_sample("ascb.bin"),
                offset=5.5,
            )
        self.assertEqual(
            "Offset must be positive integer",
            str(e.exception),
        )

    # ============================================================================
    # Format -- Testing Errors: Bad Data
    # ============================================================================

    def test_cbxp_format_raises_cbxp_error_if_data_is_too_small(self):
        with self.assertRaises(CBXPError) as e:
            cbxp.format(
                "psa",
                data=self.read_sample("ascb.bin"),
            )
        self.assertEqual(
            "Data provided is not large enough for specified control block: psa",
            str(e.exception),
        )


if __name__ == "__main__":
    unittest.main(verbosity=2, failfast=True, buffer=True)
