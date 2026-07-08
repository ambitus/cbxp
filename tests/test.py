import json
import os
import shlex
import subprocess
import sys
import unittest
from pathlib import Path

from cbxp import CBXPError, CBXPFilter, CBXPFilterOperation, cbxp

# Runs in a fresh interpreter because the control block mappings (including any
# found through CBXPPATH) are loaded once per process.
CBXPPATH_FIXTURE_SCRIPT = """
import json
import sys

from cbxp import CBXPError, cbxp

command, block, sample = sys.argv[1:4]
try:
    if command == "extract":
        data = cbxp.extract(block)
    else:
        with open(sample, "rb") as sample_file:
            data = cbxp.format(block, data=sample_file.read())
    print(json.dumps({"ok": True, "data": data}))
except CBXPError as e:
    print(json.dumps({"ok": False, "rc": e.rc, "message": str(e)}))
"""


class TestCBXP(unittest.TestCase):
    TEST_DIR = Path(__file__).resolve().parent
    SAMPLE_DIR = TEST_DIR / "samples"
    FIXTURE_DIR = TEST_DIR / "fixtures" / "cbxppath"

    # ============================================================================
    # Utility Functions
    # ============================================================================
    @staticmethod
    def read_sample(filename: str) -> bytes:
        return (TestCBXP.SAMPLE_DIR / filename).read_bytes()

    @staticmethod
    def psa_data(length: int) -> bytes:
        # Zeroed PSA buffer with the "PSA " eyecatcher (in EBCDIC) at offset 512
        data = bytearray(length)
        data[512:516] = "PSA ".encode("cp037")
        return bytes(data)

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

    def test_cbxp_extract_ldax(self):
        cbdata = cbxp.extract("ldax")
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
        self.assertIs(type(cbdata["cvtasvt"]["asvtentys"]), list)
        for entry in cbdata["cvtasvt"]["asvtentys"]:
            self.assertIs(type(entry), str)

    def test_cbxp_extract_asvt_and_include_ascb(self):
        cbdata = cbxp.extract("asvt", includes=["ascb"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["asvtentys"]), list)
        for entry in cbdata["asvtentys"]:
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

    def test_cbxp_extract_asvt_and_include_ascb_assb_ldax(self):
        cbdata = cbxp.extract("asvt", includes=["ascb.assb.ldax"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["asvtentys"]), list)
        for entry in cbdata["asvtentys"]:
            self.assertIs(type(entry), dict)
            self.assertIs(type(entry["ascbassb"]), dict)
            self.assertIs(type(entry["ascbassb"]["assbldax"]), dict)

    def test_cbxp_extract_psa_and_include_cvt_asvt_ascb_assb_ldax(self):
        cbdata = cbxp.extract("psa", includes=["cvt.asvt.ascb.assb.ldax"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["flccvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]["asvtentys"]), list)
        for entry in cbdata["flccvt"]["cvtasvt"]["asvtentys"]:
            self.assertIs(type(entry), dict)
            self.assertIs(type(entry["ascbassb"]), dict)
            self.assertIs(type(entry["ascbassb"]["assbldax"]), dict)

    def test_cbxp_extract_assb_and_include_ldax(self):
        cbdata = cbxp.extract("assb", includes=["ldax"])
        self.assertIs(type(cbdata), list)
        for entry in cbdata:
            self.assertIs(type(entry), dict)
            self.assertIs(type(entry["assbldax"]), dict)

    def test_cbxp_extract_ascb_and_include_assb_ldax(self):
        cbdata = cbxp.extract("ascb", includes=["assb.ldax"])
        self.assertIs(type(cbdata), list)
        for entry in cbdata:
            self.assertIs(type(entry), dict)
            self.assertIs(type(entry["ascbassb"]), dict)
            self.assertIs(type(entry["ascbassb"]["assbldax"]), dict)

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
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]["asvtentys"]), list)
        for entry in cbdata["flccvt"]["cvtasvt"]["asvtentys"]:
            self.assertIs(type(entry), dict)

    def test_cbxp_extract_cvt_and_include_asvt_ascb(self):
        cbdata = cbxp.extract("cvt", includes=["asvt.ascb"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["cvtasvt"]), dict)
        self.assertIs(type(cbdata["cvtasvt"]["asvtentys"]), list)
        for entry in cbdata["cvtasvt"]["asvtentys"]:
            self.assertIs(type(entry), dict)

    def test_cbxp_extract_cvt_and_include_ecvt_and_asvt(self):
        cbdata = cbxp.extract("cvt", includes=["ecvt", "asvt"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["cvtecvt"]), dict)
        self.assertIs(type(cbdata["cvtasvt"]), dict)
        self.assertIs(type(cbdata["cvtasvt"]["asvtentys"]), list)
        for entry in cbdata["cvtasvt"]["asvtentys"]:
            self.assertIs(type(entry), str)

    def test_cbxp_extract_psa_and_include_ecvt_asvt_and_cvt_asvt_ascb(
        self,
    ):
        cbdata = cbxp.extract("psa", includes=["cvt.ecvt", "cvt.asvt.ascb"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["flccvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtecvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]["asvtentys"]), list)
        for entry in cbdata["flccvt"]["cvtasvt"]["asvtentys"]:
            self.assertIs(type(entry), dict)

    def test_cbxp_extract_psa_and_include_ecvt_asvt_and_cvt_asvt_ascb_assb(
        self,
    ):
        cbdata = cbxp.extract("psa", includes=["cvt.ecvt", "cvt.asvt.ascb.assb"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["flccvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtecvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]["asvtentys"]), list)
        for entry in cbdata["flccvt"]["cvtasvt"]["asvtentys"]:
            self.assertIs(type(entry), dict)
            self.assertIs(type(entry["ascbassb"]), dict)

    def test_cbxp_extract_psa_and_include_ecvt_asvt_and_cvt_asvt_ascb_oucb(
        self,
    ):
        cbdata = cbxp.extract("psa", includes=["cvt.ecvt", "cvt.asvt.ascb.oucb"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["flccvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtecvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]["asvtentys"]), list)
        for entry in cbdata["flccvt"]["cvtasvt"]["asvtentys"]:
            self.assertIs(type(entry), dict)
            self.assertIs(type(entry["ascboucb"]), dict)

    def test_cbxp_extract_psa_and_include_cvt_recursive_wildcard(self):
        cbdata = cbxp.extract("psa", includes=["cvt.**"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["flccvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtecvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]["asvtentys"]), list)
        for entry in cbdata["flccvt"]["cvtasvt"]["asvtentys"]:
            self.assertIs(type(entry), dict)
            self.assertIs(type(entry["ascbassb"]), dict)
            self.assertIs(type(entry["ascbassb"]["assbldax"]), dict)
            # Under ** traversal, ascboucb may be a raw pointer hex string if its
            # target oucb was mid-visit when this ascb's ascboucb field was reached.
            self.assertIn(type(entry["ascboucb"]), (dict, str))

    def test_cbxp_extract_psa_and_include_cvt_wildcard(self):
        cbdata = cbxp.extract("psa", includes=["cvt.*"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["flccvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtecvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]["asvtentys"]), list)
        for entry in cbdata["flccvt"]["cvtasvt"]["asvtentys"]:
            self.assertIs(type(entry), str)

    def test_cbxp_extract_cvt_and_include_wildcard_and_asvt_wildcard(self):
        cbdata = cbxp.extract("cvt", includes=["*", "asvt.*"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["cvtecvt"]), dict)
        self.assertIs(type(cbdata["cvtasvt"]), dict)
        self.assertIs(type(cbdata["cvtasvt"]["asvtentys"]), list)
        for entry in cbdata["cvtasvt"]["asvtentys"]:
            self.assertIs(type(entry), dict)

    def test_cbxp_extract_cvt_and_include_wildcard_and_asvt_recursive_wildcard(
        self,
    ):
        cbdata = cbxp.extract("cvt", includes=["*", "asvt.**"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["cvtecvt"]), dict)
        self.assertIs(type(cbdata["cvtasvt"]), dict)
        self.assertIs(type(cbdata["cvtasvt"]["asvtentys"]), list)
        for entry in cbdata["cvtasvt"]["asvtentys"]:
            self.assertIs(type(entry), dict)
            self.assertIs(type(entry["ascbassb"]), dict)
            # Under ** traversal, ascboucb may be a raw pointer hex string if its
            # target oucb was mid-visit when this ascb's ascboucb field was reached.
            self.assertIn(type(entry["ascboucb"]), (dict, str))

    def test_cbxp_extract_ascb_and_include_assb_recursive_wildcard(self):
        cbdata = cbxp.extract("ascb", includes=["assb.**"])
        self.assertIs(type(cbdata), list)
        for entry in cbdata:
            self.assertIs(type(entry), dict)
            self.assertIs(type(entry["ascbassb"]), dict)
            self.assertIs(type(entry["ascbassb"]["assbldax"]), dict)

    def test_cbxp_extract_ascb_and_include_oucb_and_assb_recursive_wildcard(self):
        cbdata = cbxp.extract("ascb", includes=["oucb", "assb.**"])
        self.assertIs(type(cbdata), list)
        for entry in cbdata:
            self.assertIs(type(entry), dict)
            self.assertIs(type(entry["ascbassb"]), dict)
            self.assertIs(type(entry["ascbassb"]["assbldax"]), dict)
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

    def test_cbxp_extract_filter_on_ldax_tcthwm_equal(self):
        cbdata = cbxp.extract(
            "ldax",
            filters=[
                CBXPFilter(
                    "ldax_tcthwm",
                    CBXPFilterOperation.EQUAL,
                    0,
                ),
            ],
        )
        self.assertIs(type(cbdata), list)
        for entry in cbdata:
            self.assertIs(type(entry), dict)
            self.assertEqual(entry["ldax_tcthwm"], 0)

    def test_cbxp_extract_filter_on_psa_cvt_asvt_ascb_assb_ldax_tcthwm_with_include(
        self,
    ):
        cbdata = cbxp.extract(
            "psa",
            filters=[
                CBXPFilter(
                    "cvt.asvt.ascb.assb.ldax.ldax_tcthwm",
                    CBXPFilterOperation.EQUAL,
                    0,
                ),
            ],
            includes=["cvt.asvt.ascb.assb.ldax"],
        )
        self.assertIs(type(cbdata), dict)
        self.assertIn("flccvt", cbdata)
        self.assertIs(type(cbdata["flccvt"]), dict)
        self.assertIn("cvtasvt", cbdata["flccvt"])
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]), dict)
        self.assertIn("asvtentys", cbdata["flccvt"]["cvtasvt"])
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]["asvtentys"]), list)
        # Verify at least one entry matches the filter
        self.assertGreater(len(cbdata["flccvt"]["cvtasvt"]["asvtentys"]), 0)
        for entry in cbdata["flccvt"]["cvtasvt"]["asvtentys"]:
            self.assertIs(type(entry), dict)
            self.assertIn("ascbassb", entry)
            self.assertIs(type(entry["ascbassb"]), dict)
            self.assertIn("assbldax", entry["ascbassb"])
            self.assertIs(type(entry["ascbassb"]["assbldax"]), dict)
            self.assertEqual(
                entry["ascbassb"]["assbldax"]["ldax_tcthwm"],
                0,
            )

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

    def test_cbxp_extract_raises_cbxp_error_if_oucb_is_included_with_the_psa(
        self,
    ):
        # cvt.asvt.ascb is also included but is not error source
        with self.assertRaises(CBXPError) as e:
            cbxp.extract("psa", includes=["oucb", "cvt.asvt.ascb"])
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

    def test_cbxp_extract_raises_cbxp_error_if_ldax_included_with_psa(self):
        with self.assertRaises(CBXPError) as e:
            cbxp.extract("psa", includes=["ldax"])
        self.assertEqual("A bad include pattern was provided", str(e.exception))

    def test_cbxp_extract_raises_cbxp_error_if_ldax_included_with_cvt(self):
        with self.assertRaises(CBXPError) as e:
            cbxp.extract("cvt", includes=["ldax"])
        self.assertEqual("A bad include pattern was provided", str(e.exception))

    def test_cbxp_extract_raises_cbxp_error_if_ldax_included_with_ascb(self):
        with self.assertRaises(CBXPError) as e:
            cbxp.extract("ascb", includes=["ldax"])
        self.assertEqual("A bad include pattern was provided", str(e.exception))

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

    # ============================================================================
    # Extract -- Output Shape
    # ============================================================================
    def test_cbxp_extract_repeated_field_is_a_list_named_with_a_trailing_s(self):
        cbdata = cbxp.extract("asvt")
        self.assertIs(type(cbdata["asvtentys"]), list)
        self.assertNotIn("asvtenty", cbdata)
        for entry in cbdata["asvtentys"]:
            self.assertIs(type(entry), str)

    def test_cbxp_extract_pointer_field_that_is_not_included_is_a_hex_string(self):
        cbdata = cbxp.extract("cvt")
        self.assertIs(type(cbdata["cvtasvt"]), str)
        self.assertTrue(cbdata["cvtasvt"].startswith("0x"))
        self.assertNotIn("cvtasvt_ptr", cbdata)

    def test_cbxp_extract_included_pointer_field_keeps_the_raw_pointer_as_ptr_key(
        self,
    ):
        raw_pointer = cbxp.extract("cvt")["cvtasvt"]
        cbdata = cbxp.extract("cvt", includes=["asvt"])
        self.assertIs(type(cbdata["cvtasvt"]), dict)
        self.assertEqual(raw_pointer, cbdata["cvtasvt_ptr"])

    # ============================================================================
    # Extract -- Traversal
    # ============================================================================
    def test_cbxp_extract_oucb_and_include_recursive_wildcard_terminates(self):
        cbdata = cbxp.extract("oucb", includes=["**"])
        self.assertIs(type(cbdata), list)
        for entry in cbdata:
            self.assertIs(type(entry), dict)

    def test_cbxp_extract_psa_and_include_recursive_wildcard_terminates(self):
        cbdata = cbxp.extract("psa", includes=["**"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["flccvt"]), dict)

    # ============================================================================
    # Extract -- Filters on Included Control Blocks
    # ============================================================================
    def test_cbxp_extract_filter_on_included_pointer_field_matches_raw_pointer(
        self,
    ):
        raw_pointer = cbxp.extract("cvt")["cvtasvt"]
        cbdata = cbxp.extract(
            "cvt",
            includes=["asvt"],
            filters=[CBXPFilter("cvtasvt", CBXPFilterOperation.EQUAL, raw_pointer)],
        )
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["cvtasvt"]), dict)

    def test_cbxp_extract_filter_on_included_pointer_field_without_match_is_none(
        self,
    ):
        cbdata = cbxp.extract(
            "cvt",
            includes=["asvt"],
            filters=[CBXPFilter("cvtasvt", CBXPFilterOperation.EQUAL, "0x00000001")],
        )
        self.assertIsNone(cbdata)

    def test_cbxp_extract_child_filter_only_drops_the_non_matching_entries(self):
        cbdata = cbxp.extract(
            "asvt",
            includes=["ascb"],
            filters=[CBXPFilter("ascb.ascbasid", CBXPFilterOperation.EQUAL, 1)],
        )
        self.assertIs(type(cbdata["asvtentys"]), list)
        self.assertGreater(len(cbdata["asvtentys"]), 0)
        for entry in cbdata["asvtentys"]:
            self.assertEqual(entry["ascbasid"], 1)

    def test_cbxp_extract_child_filter_matching_no_entries_rejects_the_parent(self):
        cbdata = cbxp.extract(
            "asvt",
            includes=["ascb"],
            filters=[CBXPFilter("ascb.ascbasid", CBXPFilterOperation.EQUAL, 65000)],
        )
        self.assertIsNone(cbdata)

    # ============================================================================
    # Format -- Eyecatcher
    # ============================================================================
    def test_cbxp_format_returns_none_if_the_eyecatcher_does_not_match(self):
        data = self.read_sample("ascb.bin")
        cbdata = cbxp.format("ascb", data=b"\x00" * 4 + data[4:])
        self.assertIsNone(cbdata)

    def test_cbxp_format_ignores_trailing_spaces_in_the_expected_eyecatcher(self):
        # The PSA eyecatcher is "PSA " and the formatted value has the trailing
        # space trimmed.
        cbdata = cbxp.format("psa", data=self.psa_data(self.PSA_READABLE_LENGTH))
        self.assertEqual("PSA", cbdata["psapsa"])

    # ============================================================================
    # Format -- Buffer Length
    # ============================================================================
    # The back half of the PSA is fetch protected, so only (4096 - 8) / 2 bytes
    # of it are required.
    PSA_READABLE_LENGTH = 2044

    def test_cbxp_format_psa_succeeds_with_exactly_the_readable_length(self):
        cbdata = cbxp.format("psa", data=self.psa_data(self.PSA_READABLE_LENGTH))
        self.assertIs(type(cbdata), dict)

    def test_cbxp_format_psa_raises_cbxp_error_one_byte_below_the_readable_length(
        self,
    ):
        with self.assertRaises(CBXPError) as e:
            cbxp.format("psa", data=bytes(self.PSA_READABLE_LENGTH - 1))
        self.assertEqual(
            "Data provided is not large enough for specified control block: psa",
            str(e.exception),
        )

    # ============================================================================
    # Error Codes
    # ============================================================================
    def test_cbxp_error_messages_for_codes_5_through_8(self):
        expected_messages = {
            5: "A null data pointer was provided",
            6: "Error with your CBXPPATH environment variable",
            7: "Could not parse a JSON file",
            8: "Cannot reach control block",
        }
        for return_code, message in expected_messages.items():
            with self.subTest(return_code=return_code):
                error = CBXPError(return_code, "psa")
                self.assertEqual(return_code, error.rc)
                self.assertEqual(message, str(error))

    def test_cbxp_error_message_for_an_undefined_code_is_generic(self):
        self.assertEqual(
            "An unknown error occurred",
            str(CBXPError(99, "psa")),
        )

    # ============================================================================
    # CBXPPATH Fixtures (shared with test.sh)
    # ============================================================================
    @staticmethod
    def load_fixture_conf(fixture_dir: Path) -> dict[str, str]:
        conf = {}
        for line in (fixture_dir / "case.conf").read_text().splitlines():
            if not line.strip() or line.startswith("#"):
                continue
            key, _, value = shlex.split(line)[0].partition("=")
            conf[key] = value
        return conf

    def run_cbxppath_fixture(self, fixture_dir: Path):
        conf = self.load_fixture_conf(fixture_dir)
        env = {k: v for k, v in os.environ.items() if k != "CBXPPATH"}
        if "CBXPPATH" in conf:
            env["CBXPPATH"] = conf["CBXPPATH"].replace("@CASE@", str(fixture_dir))
        result = subprocess.run(
            [
                sys.executable,
                "-c",
                CBXPPATH_FIXTURE_SCRIPT,
                conf.get("COMMAND", "format"),
                conf["BLOCK"],
                str(self.SAMPLE_DIR / conf.get("SAMPLE", "")),
            ],
            env=env,
            cwd=self.TEST_DIR.parent,
            capture_output=True,
            text=True,
            check=False,
        )
        self.assertEqual(0, result.returncode, result.stderr)
        response = json.loads(result.stdout)
        if conf["EXPECT"] == "ok":
            self.assertTrue(response["ok"], response)
            output = json.dumps(response["data"], sort_keys=True, separators=(",", ":"))
            if "CONTAINS" in conf:
                self.assertIn(conf["CONTAINS"], output)
            if "NOT_CONTAINS" in conf:
                self.assertNotIn(conf["NOT_CONTAINS"], output)
            if "EXPECT_OUTPUT_FILE" in conf:
                expected = json.loads(
                    (fixture_dir / conf["EXPECT_OUTPUT_FILE"]).read_text(),
                )
                self.assertEqual(expected, response["data"])
        else:
            self.assertFalse(response["ok"], response)
            self.assertEqual(int(conf["CODE"]), response["rc"])
            self.assertEqual(conf["MESSAGE"], response["message"])


def _make_cbxppath_fixture_test(fixture_dir: Path):
    def test(self):
        self.run_cbxppath_fixture(fixture_dir)

    return test


for _fixture_dir in sorted(TestCBXP.FIXTURE_DIR.iterdir()):
    if _fixture_dir.is_dir():
        setattr(
            TestCBXP,
            f"test_cbxppath_fixture_{_fixture_dir.name}",
            _make_cbxppath_fixture_test(_fixture_dir),
        )


if __name__ == "__main__":
    unittest.main(verbosity=2, failfast=True, buffer=True)
