---
layout: default
---

# Supported Control Blocks

Control blocks that are natively supported by CBXP.
{: .fs-6 .fw-300 }

&nbsp;

{: .warning }
> _CBXP only supports extracting fields from **System-Level Control Blocks** that are **Documented Programming Interfaces** in the [z/OS MVS Data Areas](https://www.ibm.com/docs/en/zos/latest?topic=zos-mvs) documentation._

&nbsp;

{: .warning }
> _Since CBXP runs in **Problem State**, only **Non-Fetch Protected** control block data can be extracted from **Live Memory** (storage). Also, access to control block data is **NOT Serialized**, meaning that CBXP may occasionally extract control block data that is **Malformed** or otherwise **NOT Valid**._

&nbsp;

CBXP currently supports extracting the following **System-Level Control Blocks** from **Live Memory** *(storage)*.

&nbsp;

* [PSA](https://www.ibm.com/docs/en/zos/latest?topic=rqe-psa-information)
* [CVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-cvt-information)
* [ECVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-ecvt-information)
* [ASVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-asvt-information)
* [ASCB](https://www.ibm.com/docs/en/zos/latest?topic=iar-ascb-information)

&nbsp;

<pre class="mermaid">
  flowchart LR
    PSA
    PSA--> CVT
    CVT--> ECVT
    CVT--> ASVT
    ASVT--> ASCBs
    subgraph ASCBs["ASCB Array"]
        ASCB1["ASCB"]
        ASCB2["ASCB"]
        ASCB3["..."]
    end
</pre>
