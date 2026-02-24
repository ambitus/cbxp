---
layout: default
nav_order: 3
---

# Filters

How to use filters to filter repeated control block data.
{: .fs-6 .fw-300 }

&nbsp;

Some control blocks like the [ASCB](https://www.ibm.com/docs/en/zos/latest?topic=iar-ascb-information) are **Repeated**. When extracting these **Repeated** control blocks, you may want to only extract the entries that match one or more **Filters**. CBXP allows **Filters** to be provided when extracting **Repeated** control block data.

## Filter Basics

&nbsp;

**Filters** are specified as key-value pairs, where the key is a **Control Block Field** and the value is a **Filter Value**.

&nbsp;

The following example extracts all [ASSB](https://www.ibm.com/docs/en/zos/latest?topic=iar-assb-information) control blocks where the **Control Block Field** `ASSBJBNI` matches the **Filter Value** `IBMUSER`.

###### Python Script
```python
from cbxp import CBXPFilter, CBXPFilterOperation, cbxp

cbdata = cbxp(
    "assb",
    filters=[
        CBXPFilter(
            "assbjbni",
            CBXPFilterOperation.EQUAL,
            "IBMUSER"
        )
    ]
)
```

###### Shell Script
```shell
cbxp -f assbjbni=IBMUSER assb
```

## Using Multiple Filters

&nbsp;

Multiple **Filters** can be used. 

&nbsp;

The following example extracts all [ASSB](https://www.ibm.com/docs/en/zos/latest?topic=iar-assb-information) control blocks where both the **Control Block Field** `ASSBJBNI` matches the **Filter Value** `IBMUSER` and the **Control Block Field** `ASSBJBNS` matches the **Filter Value** `BPXAS`.

###### Python Script
```python
from cbxp import CBXPFilter, CBXPFilterOperation, cbxp

cbdata = cbxp(
    "assb", 
    filters=[
        CBXPFilter(
            "assbjbni",
            CBXPFilterOperation.EQUAL,
            "IBMUSER"
        ),
        CBXPFilter(
            "assbjbns",
            CBXPFilterOperation.EQUAL,
            "BPXAS"
        )
    ]
)
```

###### Shell Script
```shell
cbxp -f assbjbni=IBMUSER -f assbjbns=BPXAS ascb
```

## Using Filters With Include Patterns

&nbsp;

**Filters** can be use with control blocks included using [Include Patterns](./include_patterns). 

&nbsp;

The following example extracts all [ASCB](https://www.ibm.com/docs/en/zos/latest?topic=iar-ascb-information) and corresponding [ASSB](https://www.ibm.com/docs/en/zos/latest?topic=iar-assb-information) control blocks where the **Control Block Field** `ASSBJBNI` matches the **Filter Value** `IBMUSER`.

###### Python Script
```python
from cbxp import CBXPFilter, CBXPFilterOperation, cbxp

cbdata = cbxp(
    "ascb",
    includes=["assb"],
    filters=[
        CBXPFilter(
            "assb.assbjbni",
            CBXPFilterOperation.EQUAL,
            "IBMUSER"
        )
    ]
)
```

###### Shell Script
```shell
cbxp -i assb -f assb.assbjbni=IBMUSER ascb
```

## Using Filters With `fnmatch` Patterns

&nbsp;

[`fnmatch`](https://www.man7.org/linux/man-pages/man3/fnmatch.3.html) patterns can be used as **Filter Values**.

&nbsp;

The following example extracts all [ASSB](https://www.ibm.com/docs/en/zos/latest?topic=iar-assb-information) control blocks where the **Control Block Field** `ASSBJBNI` matches the **Filter Value** `JOB*`.

###### Python Script
```python
from cbxp import CBXPFilter, CBXPFilterOperation, cbxp

cbdata = cbxp(
    "assb", 
    filters=[
        CBXPFilter(
            "assbjbni",
            CBXPFilterOperation.EQUAL,
            "JOB*"
        )
    ]
)
```

###### Shell Script
```shell
cbxp -f 'assbjbni=JOB*' assb
```

## Filtering Numeric Data

&nbsp;

`>`, `>=`, `<`, and `>=` can be used when filtering **Numeric Control Block Fields**.

&nbsp;

The following example extracts all [ASSB](https://www.ibm.com/docs/en/zos/latest?topic=iar-assb-information) control blocks where the **Control Block Field** `ASSB_TIME_ON_CP` is **Less Than** `30000`.

###### Python Script
```python
from cbxp import CBXPFilter, CBXPFilterOperation, cbxp

cbdata = cbxp(
    "assb",
    filters=[
        CBXPFilter(
            "assb_time_on_cp",
            CBXPFilterOperation.LESS_THAN,
            30000
    ]
)
```

###### Shell Script
```shell
cbxp -f 'assb_time_on_cp<3000' assb
```
