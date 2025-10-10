---
layout: default
parent: Interfaces
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

## `cbxp.cbxp()`

```python
def cbxp(
        control_block: str, 
        includes: list[str] = [], 
        debug: bool = False
) -> dict:
```

### 📄 Description
Extract **Control Blocks** from **Live Memory** *(storage)* and post-process them into a **Python Dictionary**.

### 📥 Parameters

* `control_block` <br>
  The name of the **Control Block** to extract.

* `includes` <br>
  A **List** of [Include Patterns](../../include_patterns) that describe **Additional Control Blocks** to include that are accessible from the **Root Control Block** being extracted.

* `debug` <br>
  A **Boolean** that if set to `True` indicates that **Debug Messages** should be printed. If set to `False`, no **Debug Messages** will be printed.

### 📤 Returns
* `dict` <br>
  A **Python Dictionary** that contains the **Extracted Control Block Data** requested.

### ❌ Raises
* `CBXPError` <br>
  Raises `CBXPError` when an error or condition that prevents the operation from completing successfully occurs.

### 💻 Examples

The following example extracts the [PSA](https://www.ibm.com/docs/en/zos/latest?topic=rqe-psa-information) control block and prints **Debug Messages**.

###### Python Script
```python
from cbxp import cbxp

cbdata = cbxp("psa", debug=True)
```

&nbsp;

The following example extracts the [CVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-cvt-information), includes the [ECVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-ecvt-information) and the [ASVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-asvt-information), and includes all **Known Control Blocks** that are pointed to directly by the [ASVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-asvt-information).

###### Python Script
```python
from cbxp import cbxp

cbdata = cbxp("cvt", includes=["ecvt", "asvt.*"])
```

&nbsp;

The followng example extracts all [ASCB](https://www.ibm.com/docs/en/zos/latest?topic=iar-ascb-information) control blocks and uses the built-in Python library `json` to convert the resulting Python dictionary into a **JSON String**.

###### Python Script
```python
import json
from cbxp import cbxp

cbdata = cbxp("ascb")
cbjson = json(cbdata, indent=2)
```
