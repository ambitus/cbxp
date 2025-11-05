import unittest
from cbxp import cbxp, CBXPError

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

    def test_cbxp_include_can_extract_cvt_and_include_pattern_ecvt_and_asvt(self):
        cbdata = cbxp("cvt", includes=["ecvt", "asvt"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["cvtecvt"]), dict)
        self.assertIs(type(cbdata["cvtasvt"]), dict)
        self.assertIs(type(cbdata["cvtasvt"]["asvtenty"]), list)
        for entry in cbdata["cvtasvt"]["asvtenty"]:
            self.assertIs(type(entry), str)

    def test_cbxp_include_can_extract_psa_and_include_pattern_ecvt_asvt_and_cvt_asvt_ascb(self):
        cbdata = cbxp("psa", includes=["cvt.ecvt", "cvt.asvt.ascb"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["flccvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtecvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]["asvtenty"]), list)
        for entry in cbdata["flccvt"]["cvtasvt"]["asvtenty"]:
            self.assertIs(type(entry), dict)
    
    def test_cbxp_can_extract_psa_and_include_cvt_recursive_wildcard(self):
        cbdata = cbxp("psa", includes=["cvt.**"])
        self.assertIs(type(cbdata), dict)
        self.assertIs(type(cbdata["flccvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtecvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]), dict)
        self.assertIs(type(cbdata["flccvt"]["cvtasvt"]["asvtenty"]), list)
        for entry in cbdata["flccvt"]["cvtasvt"]["asvtenty"]:
            self.assertIs(type(entry), dict)
    
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
        self.assertEqual("Unknown control block 'unknown' was specified.", str(e.exception))

    # ============================================================================
    # Errors: Bad Include Patterns
    # ============================================================================
    def test_cbxp_raises_cbxp_error_if_asvt_ascb_is_included_when_extracting_the_psa(self):
        with self.assertRaises(CBXPError) as e:
            cbxp("psa", includes=["asvt.ascb"])
        self.assertEqual("A bad include pattern was provided", str(e.exception))

    def test_cbxp_raises_cbxp_error_if_ascb_is_included_when_extracting_the_psa(self):
        with self.assertRaises(CBXPError) as e:
            cbxp("psa", includes=["ascb"])
        self.assertEqual("A bad include pattern was provided", str(e.exception))

    def test_cbxp_raises_cbxp_error_if_ecvt_is_included_when_extracting_the_ascb(self):
        with self.assertRaises(CBXPError) as e:
            cbxp("ascb", includes=["ecvt"])
        self.assertEqual("A bad include pattern was provided", str(e.exception))
    
    def test_cbxp_raises_cbxp_error_if_ect_ecvt_and_cvt_ascb_is_included_when_extracting_the_psa(self):
        with self.assertRaises(CBXPError) as e:
            cbdata = cbxp("psa", includes=["cvt.ecvt", "cvt.ascb"])
        self.assertEqual("A bad include pattern was provided", str(e.exception))

    def test_cbxp_raises_cbxp_error_if_ecvt_and_cvt_asvt_ascb_is_included_when_extracting_the_psa(self):
        with self.assertRaises(CBXPError) as e:
            cbdata = cbxp("psa", includes=["ecvt", "cvt.asvt.ascb"])
        self.assertEqual("A bad include pattern was provided", str(e.exception))

    def test_cbxp_raises_cbxp_error_if_cvt_is_included_when_extracting_the_cvt(self):
        with self.assertRaises(CBXPError) as e:
            cbdata = cbxp("cvt", includes=["cvt"])
        self.assertEqual("A bad include pattern was provided", str(e.exception))

    def test_cbxp_raises_cbxp_error_when_pattern_cannot_contain_comma_1(self):
        with self.assertRaises(CBXPError) as e:
            cbdata = cbxp("cvt", includes=["asvt,ascb"])
        self.assertEqual("Include patterns cannot contain commas", str(e.exception))

    def test_cbxp_raises_cbxp_error_when_pattern_cannot_contain_comma_2(self):
        with self.assertRaises(CBXPError) as e:
            cbdata = cbxp("cvt", includes=["asvt,as"])
        self.assertEqual("Include patterns cannot contain commas", str(e.exception))


if __name__ == "__main__":
    unittest.main(verbosity=2, failfast=True, buffer=True)
