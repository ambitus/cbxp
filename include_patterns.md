---
layout: default
nav_order: 2
---

# Include Patterns

How to use include patterns to include additional control blocks.
{: .fs-6 .fw-300 }

&nbsp;

{: .warning }
> _When using **Wildcards**, include patterns **MUST** end with the wildcard._
>
> &nbsp;
> 
> | ❌ | ✅ |
> | `**.ascb` | `cvt.*` |
> | `cvt.*.ascb` | `cvt.asvt.**` |
> | `*.asvt.*` | `*` |
> | `cvt.**.ascb` | `**` |
>

&nbsp;

{: .warning }
> _When using the [Shell Interface](../interfaces/shell) for CBXP, ensure that **Include Patterns** that contain **Wildcards** are surrounded with quotes._
>
> &nbsp;
>
> ❌
> ```shell
> cbxp -i cvt.* psa
> ```
>
> &nbsp;
>
> ✅
> ```shell
> cbxp -i "cvt.*" psa
> ```

&nbsp;

**Include Patterns** provide a method for including **Additional Control Blocks** that can be found from the **Root Control Block** being extracted.

## Include Pattern Basics

&nbsp;

**Include Patterns** are specified as **Dot Delimited** patterns describing control blocks to include starting from the **Root Control Block** being extracted.

&nbsp;

In the following example, `psa` is the **Root Control Block** and `cvt.ecvt` is the **Include Pattern**.

&nbsp;

The following example extracts the [PSA](https://www.ibm.com/docs/en/zos/latest?topic=rqe-psa-information), includes the [CVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-cvt-information), and includes the [ECVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-ecvt-information) with the [CVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-cvt-information).

###### Python Script
```python
from cbxp import cbxp

cbdata = cbxp("psa", includes=["cvt.ecvt"])
```

###### Shell Script
```python
cbxp -i cvt.ecvt psa
```

&nbsp;

<pre class="mermaid">
  flowchart LR
    PSA
    PSA--> CVT
    CVT--> ECVT
</pre>

## Using Multiple Include Patterns

&nbsp;

**Multiple Include Patterns** can be specified. 

&nbsp;

The following example extracts the [CVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-cvt-information), and includes the [ECVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-ecvt-information) and the [ASVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-asvt-information).

###### Python Script
```python
from cbxp import cbxp

cbdata = cbxp("cvt", includes=["ecvt", "asvt"])
```

###### Shell Script
```python
cbxp -i ecvt -i asvt cvt
```

&nbsp;

<pre class="mermaid">
  flowchart LR
    CVT
    CVT--> ECVT
    CVT--> ASVT
</pre>

## Using Wildcards

&nbsp;

CBXP also supports **Wildcarding**. Coding `*` at the very end of your include pattern tells CBXP to include all **Known Control Blocks** pointed to directly by the **Preceding Dot Delimited Control Block Name** or the **Root Control Block** in the case where the include pattern is just `*`.

&nbsp;

The following example extracts the [CVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-cvt-information) and includes all **Known Control Blocks** that are pointed to directly by it. Additionally, `asvt.*` causes the [ASVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-asvt-information) and all **Known Control Blocks** that are pointed to directly by the [ASVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-asvt-information) to also be included.

###### Python Script
```python
from cbxp import cbxp

cbdata = cbxp("cvt", includes=["*", "asvt.*"])
```

###### Shell Script
```python
cbxp -i "*" -i "asvt.*" cvt
```

&nbsp;

<pre class="mermaid">
  flowchart LR
    CVT
    CVT--> ECVT
    CVT--> ASVT
    ASVT--> ASCBs
    subgraph ASCBs["ASCB Array"]
        ASCB1["ASCB"]
        ASCB2["ASCB"]
        ASCB3["ASCB"]
    end
</pre>

## Using Recursive Wildcards

&nbsp;

**Recursive Wildcarding** is also supported. Coding `**` at the very end of your include pattern tells CBXP to include all **Known Control Blocks** that can be found starting from the **Preceding Dot Delimited Control Block Name** or the **Root Control Block** in the case where the include pattern is just `**`.

&nbsp;

The following example extracts the [PSA](https://www.ibm.com/docs/en/zos/latest?topic=rqe-psa-information), includes the [CVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-cvt-information), and includes **All Known Control Blocks** that can be found starting from the [CVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-cvt-information).

###### Python Script
```python
from cbxp import cbxp

cbdata = cbxp("psa", includes=["cvt.**"])
```

###### Shell Script
```python
cbxp -i "cvt.**" psa
```

&nbsp;

<pre class="mermaid">
  flowchart TD
    PSA
    PSA--> CVT
    CVT--> ECVT
    CVT--> ASVT
    ASVT--> ASCBs
    subgraph ASCBs["ASCB Array"]
        ASCB1["ASCB"]
        ASCB1--> ASSB1["ASSB"]
        ASCB1--> ASXB1["ASXB"]
        ASCB2["ASCB"]
        ASCB2--> ASSB2["ASSB"]
        ASCB2--> ASXB2["ASXB"]
        ASCB3["ASCB"]
        ASCB3--> ASSB3["ASSB"]
        ASCB3--> ASXB3["ASXB"]
    end
</pre>
