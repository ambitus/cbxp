[![Version](https://img.shields.io/pypi/v/cbxp?label=alpha)](https://pypi.org/project/cbxp/#history)
[![Python Versions](https://img.shields.io/pypi/pyversions/cbxp)](https://pypi.org/project/cbxp/)
[![Downloads](https://img.shields.io/pypi/dm/cbxp)](https://pypistats.org/packages/cbxp)

![CBXP Logo](https://raw.githubusercontent.com/ambitus/cbxp/refs/heads/main/logo.svg)

# CBXP (Control Block EXPlorer) 

A unified and standardized interface for extracting z/OS control block data.

## Description

z/OS Control Blocks are in-memory data structures that describe and control countless process, operating system components, and subsystems. Control blocks are unbiquitous on z/OS, but not very straight forward to access and extract information from. The mission of CBXP *(Control Block EXPlorer)* is to make it easy to extract z/OS control block data using industry standard tools and methodologies. CBXP accomplishes this by implementing a **C/C++ XPLINK ASCII** interface for extracting control blocks and post processing them into **JSON**. This makes it straight forward to integrate with industry standard programming languages and tools, which generally have well documented and understood foreign language intefaces for C/C++, and native and or third party JSON support that makes working with JSON data easy.

CBXP is the successor to the existing [cbxplorer](https://github.com/ambitus/cbexplorer) project. CBXP mainly improves upon this existing work by being implementing in C/C++ so that it is not limited to a specific programming language or tool. CBXP also focuses heavily on providing an interface that is simple and straight forward to use.

## Getting Started

### Minimum z/OS & Language Versions
Currently, CBXP is being developed on **z/OS 3.1**. We hope to eventually support all z/OS versions that are fully supported by IBM.
* [z/OS Product Lifecycle](https://www.ibm.com/support/pages/lifecycle/search/?q=5655-ZOS,%205650-ZOS)

All versions of the **IBM Open Enterprise SDK for Python** that are fully supported by IBM are supported by CBXP.
* [IBM Open Enterprise SDK for Python Product Lifecycle](https://www.ibm.com/support/pages/lifecycle/search?q=5655-PYT)

### Dependencies
* **z/OS Language Environment Runtime Support**: CBXP is compiled using the **IBM Open XL C/C++ 2.1** compiler, which is still fairly new and requires **z/OS Language Environment** service updates for runtime support.
  * More information can be found in section **5.2.2.2 Operational Requisites** on page **9** in the [Program Directory for IBM Open XL C/C++ 2.1 for z/OS](https://publibfp.dhe.ibm.com/epubs/pdf/i1357012.pdf).

### Interfaces
Currently, the following interfaces are provided for CBXP. Additional interfaces can be added in the future if there are use cases for them.
* [Python Interface](https://ambitus.github.io/cbxp/interfaces/python)
* [Shell Interface](https://ambitus.github.io/cbxp/interfaces/shell)

### Supported Control Blocks

Currently, CBXP only has support for extracting a handful of **System-Level Control Blocks** from **Live Memory** *(storage)*. See [Supported Control Blocks](https://ambitus.github.io/cbxp/supported_control_blocks) for more details.

## Help
* [GitHub Discussions](https://github.com/ambitus/cbxp/discussions)

## Authors
* Leonard J. Carcaramo Jr: lcarcaramo@ibm.com
* Elijah Swift: elijah.swift@ibm.com
* Varun Chennamadhava: varunchennamadhava@ibm.com

## Maintainers
* Leonard J. Carcaramo Jr: lcarcaramo@ibm.com
* Elijah Swift: elijah.swift@ibm.com
* Varun Chennamadhava: varunchennamadhava@ibm.com
