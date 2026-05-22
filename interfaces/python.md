---
layout: default
parent: Interfaces
nav_order: 1
---

# Python

The following Python interface is provided to facilitate exploitation of CBXP by Python callers.
{: .fs-6 .fw-300 }

&nbsp;

{: .note }
> _The **Python Interface** for CBXP can be installed from [PyPi](https://pypi.org/project/cbxp/) using `pip`._
> 
> <br>
>
> ```shell
> python3 -m pip install cbxp
> ```
> _The **Python Interface** for CBXP may also optionally be downloaded from [GitHub](https://github.com/ambitus/cbxp/releases)._

## `cbxp.extract()`

```python
def extract(
    control_block: str,
    includes: list[str] = None,
    filters: list[CBXPFilter] = None,
    debug: bool = False,
) -> dict:
```

### 📄 Description
Extract and format **Control Block Data** from **Live Memory**.

### 📥 Parameters

* `control_block` <br>
  The name of the **Control Block** to extract.

* `includes` <br>
  A **List** of [Include Patterns](../../include_patterns) that describe **Additional Control Blocks** to include that are accessible from the **Root Control Block** being extracted.

* `filters` <br>
  A **List** of [Filters](../../filters) that are used to filter the entries returned in **Repeated** control block data.

* `debug` <br>
  A **Boolean** that if set to `True` indicates that **Debug Messages** should be printed. If set to `False`, no **Debug Messages** will be printed.

### 📤 Returns
* `dict` <br>
  A **Python Dictionary** that contains **Formatted Control Block Data**.

### ❌ Raises
* `CBXPError` <br>
  Raises `CBXPError` when an error or condition that prevents the operation from completing successfully occurs.

### 💻 Examples

The following example extracts the [PSA](https://www.ibm.com/docs/en/zos/latest?topic=rqe-psa-information) control block from **Live Memory** and prints **Debug Messages**.

###### Python Script
```python
from cbxp import cbxp

cbdata = cbxp.extract("psa", debug=True)
```

&nbsp;

The following example extracts the [CVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-cvt-information) control block from **Live Memory**, includes the [ECVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-ecvt-information) and the [ASVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-asvt-information), and includes all **Known Control Blocks** that are pointed to directly by the [ASVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-asvt-information).

###### Python Script
```python
from cbxp import cbxp

cbdata = cbxp.extract("cvt", includes=["ecvt", "asvt.*"])
```

&nbsp;

The following example extracts all [ASSB](https://www.ibm.com/docs/en/zos/latest?topic=iar-assb-information) control blocks from **Live Memory** where the **Control Block Field** `ASSBJBNI` matches the **Filter Value** `IBMUSER`. The built-in Python library `json` is then used to convert the resulting Python dictionary into a **JSON String**.

###### Python Script
```python
import json
from cbxp import CBXPFilter, CBXPFilterOperation, cbxp

cbdata = cbxp.extract(
    "assb",
    filters=[
        CBXPFilter(
            "assbjbni",
            CBXPFilterOperation.EQUAL,
            "IBMUSER"
        )
    ]
)

cbjson = json(cbdata, indent=2)
```

## `cbxp.format()`

```python
def format(
    control_block: str,
    data: bytes,
    offset: int = None,
    debug: bool = False,
) -> dict:
```

### 📄 Description
Format **Caller-Provided Control Block Data** from **Live Memory**.

### 📥 Parameters

* `control_block` <br>
  The name of the **Control Block** to format.

* `data` <br>
  A **Bytes Object** containing the **Raw Control Block Data** to format.

* `offset` <br>
  An **Optional** offset into the provided **Raw Control Block Data** to start formatting at.

* `debug` <br>
  A **Boolean** that if set to `True` indicates that **Debug Messages** should be printed. If set to `False`, no **Debug Messages** will be printed.

### 📤 Returns
* `dict` <br>
  A **Python Dictionary** that contains the **Formatted Control Block Data**.

### ❌ Raises
* `CBXPError` <br>
  Raises `CBXPError` when an error or condition that prevents the operation from completing successfully occurs.

### 💻 Examples

The following example formats **Caller-Provided** [ASCB](https://www.ibm.com/docs/en/zos/latest?topic=iar-ascb-information) control block data.

###### Python Script
```python
from cbxp import cbxp

with open("ascb.bin", "rb") as f:
    data = f.read()

cbdata = cbxp.format("ascb", data)
```

&nbsp;

The following example formats **Caller-Provided** [ASCB](https://www.ibm.com/docs/en/zos/latest?topic=iar-ascb-information) control block data at an offset of `0x40` bytes.

###### Python Script
```python
from cbxp import cbxp

with open("ascboffset.bin", "rb") as f:
    data = f.read()

cbdata = cbxp.format("ascb", data, offset=0x40)
```
