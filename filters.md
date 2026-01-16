---
layout: default
nav_order: 3
---

# Filters

How to use filters to filter repeated control block data.
{: .fs-6 .fw-300 }

&nbsp;

{: .note }
> _**Filters** can **ONLY** be used with **Repeated** control blocks like the [ASCB](https://www.ibm.com/docs/en/zos/latest?topic=iar-ascb-information). **Filters** cannot be used with **Singleton** control blocks like the [PSA](https://www.ibm.com/docs/en/zos/latest?topic=rqe-psa-information)._

&nbsp;

Some control blocks like the [ASCB](https://www.ibm.com/docs/en/zos/latest?topic=iar-ascb-information) are **Repeated**. When extracting these **Repeated** control blocks, you may want to only extract the entries that match one or more **Filters**. CBXP allows **Filters** to be provided when extracting **Repeated** control block data.

## Filter Basics

&nbsp;

**Filters** are specified as key-value pairs, where the key is a **Control Block Field** and the value is a **Filter Pattern**.

&nbsp;

The following example extracts all [ASSB](https://www.ibm.com/docs/en/zos/latest?topic=iar-assb-information) control blocks where the **Control Block Field** `ASSBJBNI` matches the **Filter Pattern** `IBMUSER`.

###### Python Script
```python
from cbxp import cbxp

cbdata = cbxp("assb", filters=["assbjbni=IBMUSER"])
```

###### Shell Script
```shell
cbxp -f assbjbni=IBMUSER assb
```

## Using Multiple Filters

&nbsp;

Multiple **Filters** can be used. 

&nbsp;

The following example extracts all [ASSB](https://www.ibm.com/docs/en/zos/latest?topic=iar-assb-information) control blocks where both the **Control Block Field** `ASSBJBNI` matches the **Filter Pattern** `IBMUSER` and the **Control Block Field** `ASSBJBNS` matches the **Filter Pattern** `BPXAS`.

###### Python Script
```python
from cbxp import cbxp

cbdata = cbxp("ascb", filters=["assbjbni=IBMUSER", "assbjbns=BPXAS"])
```

###### Shell Script
```shell
cbxp -i assb -f assb.assbjbni=IBMUSER ascb
```

## Using Filters With Include Patterns

&nbsp;

**Filters** can be use with control blocks included using [Include Patterns](./include_patterns). 

&nbsp;

The following example extracts all [ASCB](https://www.ibm.com/docs/en/zos/latest?topic=iar-ascb-information) and corresponding [ASSB](https://www.ibm.com/docs/en/zos/latest?topic=iar-assb-information) control blocks where the **Control Block Field** `ASSBJBNI` matches the **Filter Pattern** `IBMUSER`.

###### Python Script
```python
from cbxp import cbxp

cbdata = cbxp("ascb", includes=["assb"], filters=["assb.assbjbni=IBMUSER"])
```

###### Shell Script
```shell
cbxp -i assb -f assb.assbjbni=IBMUSER ascb
```

## Using Filters With `fnmatch` Patterns

&nbsp;

[`fnmatch`](https://www.man7.org/linux/man-pages/man3/fnmatch.3.html) patterns can be used with **Filter Patterns**.

&nbsp;

The following example extracts all [ASSB](https://www.ibm.com/docs/en/zos/latest?topic=iar-assb-information) control blocks where the **Control Block Field** `ASSBJBNI` matches the **Filter Pattern** `JOB*`.

###### Python Script
```python
from cbxp import cbxp

cbdata = cbxp("assb", filters=["assbjbni=JOB*"])
```

###### Shell Script
```shell
cbxp -f assbjbni=IBMUSER assb
```

## Filtering Numeric Data

&nbsp;

`>`, `>=`, `<`, and `>=` can be used when filtering **Numeric Control Block Fields**.

&nbsp;

The following example extracts all [ASSB](https://www.ibm.com/docs/en/zos/latest?topic=iar-assb-information) control blocks where the **Control Block Field** `ASSB_TIME_ON_CP` is **Less Than** `30000`.

###### Python Script
```python
from cbxp import cbxp

cbdata = cbxp("assb", filters=["assb_time_on_cp<30000"])
```

###### Shell Script
```shell
cbxp -f 'assb_time_on_cp>3000' assb
```
