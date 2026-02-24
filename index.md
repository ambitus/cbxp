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

## Minimum z/OS & Language Versions

Currently, CBXP is being developed on **z/OS 3.1**. We hope to eventually support all z/OS versions that are fully supported by IBM.
* [z/OS Product Lifecycle](https://www.ibm.com/support/pages/lifecycle/search/?q=5655-ZOS,%205650-ZOS)

All versions of the **IBM Open Enterprise SDK for Python** that are fully supported by IBM are supported by CBXP.
* [IBM Open Enterprise SDK for Python Product Lifecycle](https://www.ibm.com/support/pages/lifecycle/search?q=5655-PYT)

## Dependencies

* **z/OS Language Environment Runtime Support**: CBXP is compiled using the **IBM Open XL C/C++ 2.1** compiler, which is still fairly new and requires **z/OS Language Environment** service updates for runtime support.
  * More information can be found in section **5.2.2.2 Operational Requisites** on page **9** in the [Program Directory for IBM Open XL C/C++ 2.1 for z/OS](https://publibfp.dhe.ibm.com/epubs/pdf/i1357012.pdf).


## Interfaces
Currently, the following interfaces are provided for CBXP. Additional interfaces can be added in the future if there are use cases for them.
* [Python Interface](./interfaces/python)
* [Shell Interface](./interfaces/shell)
* [C/C++ Interface](./interfaces/c_cpp)


## Supported Control Blocks

Currently, CBXP only has support for extracting a handful of **System-Level Control Blocks** from **Live Memory** *(storage)*. See [Supported Control Blocks](./supported_control_blocks) for more details.

&nbsp;

We plan on adding support for extracting control blocks from **Files** and **Data Sets**, and extracting control blocks based on a **User Defined Structure**. We also plan on continuously expanding upon the following list of natively supported control blocks. The community is encouraged to assist in these efforts. See the [Contribution Guidelines](https://github.com/ambitus/cbxp/blob/main/CONTRIBUTING.md) for more details.


## Architecture

&nbsp;

<pre class="mermaid">
  flowchart LR
    python(Python Interface) ---CBXP
    style python fill:#ffcb3c,color:#000,stroke:#ffcb3c
    shell(Shell Interface) ---CBXP
    style shell fill:#33cc22,color:#000,stroke:#33cc22
    C(C/C++ Interface) ---CBXP
    style C fill:#01559e,color:#fff,stroke:#01559e
    subgraph C/C+
        CBXP(["CBXP (64-bit XPLINK ASCII)"])
        style CBXP fill:#0096ff,color:#fff,stroke:#0096ff
    end
    subgraph "Live Memory (Storage)"
        CBXP--- control_blocks_storage@{ shape: procs, label: "Control Blocks"}
        style control_blocks_storage color:#fff,stroke:#fff
    end
</pre>
