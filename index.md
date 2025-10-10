---
layout: home
has_toc: false
---

![CBXP Logo](/assets/images/logo.svg)

&nbsp;

A unified and standardized interface for extracting z/OS control block data. 
{: .fs-6 .fw-300 }

&nbsp;

{: .development_status }
> _**CBXP** is currently in **Alpha**, meaning that some core functionality may still be missing, unstable, and or subject to change._

## Mission Statement

z/OS Control Blocks are in-memory data structures that describe and control countless process, operating system components, and subsystems. Control blocks are unbiquitous on z/OS, but not very straight forward to access and extract information from. The mission of CBXP *(Control Block EXPlorer)* is to make it easy to extract z/OS control block data using industry standard tools and methodologies. CBXP accomplishes this by implementing a **C/C++ XPLINK ASCII** interface for extracting control blocks and post processing them into **JSON**. This makes it straight forward to integrate with industry standard programming languages and tools, which generally have well documented and understood foreign language intefaces for C/C++, and native and or third party JSON support that makes working with JSON data easy.

&nbsp;

CBXP is the successor to the existing [cbxplorer](https://github.com/ambitus/cbexplorer) project. CBXP mainly improves upon this existing work by being implementing in C/C++ so that it is not limited to a specific programming language or tool. CBXP also focuses heavily on providing an interface that is simple and straight forward to use.

&nbsp;

You can find information about system level control blocks in the [z/OS MVS Data Areas](https://www.ibm.com/docs/en/zos/latest?topic=zos-mvs) documentation.

## Minimum z/OS & Language Versions

Currently, CBXP is being developed on **z/OS 3.1**. We hope to eventually support all z/OS versions that are fully supported by IBM.
* [z/OS Product Lifecycle](https://www.ibm.com/support/pages/lifecycle/search/?q=5655-ZOS,%205650-ZOS)

All versions of the **IBM Open Enterprise SDK for Python** that are fully supported by IBM are supported by CBXP.
* [IBM Open Enterprise SDK for Python Product Lifecycle](https://www.ibm.com/support/pages/lifecycle/search?q=5655-PYT)

## Dependencies

* **z/OS Language Environment Runtime Support**: CBXP is compiled using the **IBM Open XL C/C++ 2.1** compiler, which is still fairly new and requires **z/OS Language Environment** service updates for runtime support.
  * More information can be found in section **5.2.2.2 Operational Requisites** on page **9** in the [Program Directory for IBM Open XL C/C++ 2.1 for z/OS](https://publibfp.dhe.ibm.com/epubs/pdf/i1357012.pdf).


## Supported Control Blocks
Currently, CBXP supports the following **System Level Control Blocks**. CBXP also currently only supports extracting control blocks from **Live Memory** *(storage)*. We plan on adding support for extracting control blocks from **Files** and **Data Sets**, and extracting control blocks based on a **User Defined Structure**. We also plan on continuously expanding upon the following list of natively supported control blocks. The community is encouraged to assist in these efforts. See the [Contribution Guidelines](https://github.com/ambitus/cbxp/blob/main/CONTRIBUTING.md) for more details.

&nbsp;

* `psa`
* `cvt`
* `ecvt`
* `asvt`
* `ascb`

## Python Interface
A **Python Interface** for CBXP is available for installation from [PyPi](https://pypi.org/project/cbxp/) or download from [GitHub](https://github.com/ambitus/cbxp/releases).

### Python Installation *(PyPi)*
```shell
python3 -m pip install cbxp
```

### Python Usage

&nbsp;

{: .note }
_See the [Python Interface](./interfaces/python) documentation for more details._

&nbsp;

```python
from cbxp import cbxp
psa = cbxp("psa")
```

## Shell Interface
A **Shell Interface** for CBXP is available for download from [GitHub](https://github.com/ambitus/cbxp/releases).

### Installation *(GitHub)*
Wherever you **Install**/**Extract** the **CBXP Pax File**, make sure to add the path to the `bin` directory where the `cbxp` executable resides to `PATH` in `/etc/profile` *(global)* or `~/.profile`/`~/.bashrc` *(local/individual)*.

### Shell Usage

&nbsp;

{: .note }
_See the [Shell Interface](./interfaces/shell) documentation for more details._

&nbsp;

```shell
cbxp psa
```

## Architecture

&nbsp;

{: .warning }
> _CBXP currently does **NOT** support extracting control blocks from **Files** and **Data Sets**._

&nbsp;

<pre class="mermaid">
  flowchart LR
    python(Python Interface) ---CBXP
    style python fill:#ffcb3c,color:#000,stroke:#ffcb3c
    shell(Shell Interface) ---CBXP
    style shell fill:#33cc22,color:#000,stroke:#33cc22
    subgraph C/C+
        CBXP(["CBXP (64-bit XPLINK ASCII)"])
        style CBXP fill:#0096ff,color:#fff,stroke:#0096ff
    end
    subgraph "Live Storage (Memory)"
        CBXP--- control_blocks_storage@{ shape: procs, label: "Control Blocks"}
        style control_blocks_storage color:#fff,stroke:#fff
    end
    subgraph "Data Sets"
        CBXP--- control_blocks_data_set[("Control Blocks")]
        style control_blocks_data_set color:#fff,stroke:#fff
    end
    subgraph "Files"
        CBXP--- control_blocks_file[("Control Blocks")]
        style control_blocks_file color:#fff,stroke:#fff
    end
</pre>
