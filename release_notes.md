---
layout: default
nav_order: 4
---

# Release Notes

## v0.0.3 - March 20, 2026
* Add support for extracting [ASSB](https://www.ibm.com/docs/en/zos/latest?topic=iar-assb-information) control block data ([#17](https://github.com/ambitus/cbxp/pull/17))
* Add support for extracting [OUCB](https://www.ibm.com/docs/en/zos/latest?topic=rqe-oucb-information) control block data ([#26](https://github.com/ambitus/cbxp/pull/26))
* Add support for filtering repeated control block data using [Filters](../filters) ([#20](https://github.com/ambitus/cbxp/pull/20), [#28](https://github.com/ambitus/cbxp/pull/28))
* Start providing a **Static Library** distribution to facilitate exploitation by [C/C++](../interfaces/c_cpp) callers ([#29](https://github.com/ambitus/cbxp/pull/29))
* End support for **Python 3.12** since IBM will end support for the [IBM Open Enterprise SDK for Python 3.12](https://www.ibm.com/docs/en/python-zos/3.12.0) in April 2026 ([#33](https://github.com/ambitus/cbxp/pull/33))
* Add support for **Python 3.14** ([#33](https://github.com/ambitus/cbxp/pull/33))
* Start compiling all distributions of CBXP with the [IBM Open XL C/C++ 2.2](https://www.ibm.com/docs/en/open-xl-c-cpp-zos/2.2.0) compiler ([#33](https://github.com/ambitus/cbxp/pull/33))

## v0.0.2 - November 6, 2025
Initial alpha release.