---
layout: default
parent: Interfaces
nav_order: 2
---

# Shell

The following shell interface is provided to facilitate exploitation of CBXP by shell callers.
{: .fs-6 .fw-300 }

&nbsp;

{: .note }
> _The **Shell Interface** for CBXP downloaded from [GitHub](https://github.com/ambitus/cbxp/releases)._

&nbsp;

{: .note }
> _Wherever you **Install**/**Extract** the **CBXP Pax File**, make sure to add the path to the `bin` directory where the `cbxp` executable resides to `PATH` and add the path to the `lib` directory where the `libcbxp.so` library resides to `LIBPATH` in `/etc/profile` (global) or `~/.profile`/`~/.bashrc` (local/individual)._

&nbsp;

### 📄 Description

Extract **Control Blocks** from **Live Memory** *(storage)*, post-process them into **JSON**, and write the result JSON to the `stdout` file descriptor.

&nbsp;

```
cbxp [options] [CONTROL_BLOCK]
```

* `-d`, `--debug` <br>
  Print **Debug Messages**.

* `-i`, `--include <pattern>` <br>
  **Include** control blocks that are **Accessible** from the **Root Control Block** being extracted using an [Include Pattern](../../include_patterns).

* `-f`, `--filter` <br>
  **Filter** repeated control block data using a [Filter Pattern](../../filters).

* `-v`, `--version` <br>
  Display **Version Information**.

* `-h`, `--help` <br>
  Display **Usage Information**.

### 💻 Examples

The following example extracts the [PSA](https://www.ibm.com/docs/en/zos/latest?topic=rqe-psa-information) control block and prints **Debug Messages**.

###### Shell Script
```shell
cbxp -d psa
```

&nbsp;

The following example extracts the [CVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-cvt-information), includes the [ECVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-ecvt-information) and the [ASVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-asvt-information), and includes all **Known Control Blocks** that are pointed to directly by the [ASVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-asvt-information).

###### Shell Script
```shell
cbxp -i ecvt -i 'asvt.*' cvt
```

&nbsp;

The following example extracts all [ASSB](https://www.ibm.com/docs/en/zos/latest?topic=iar-assb-information) control blocks where both the **Control Block Field** `ASSBJBNI` matches the **Filter Pattern** `IBMUSER` and the **Control Block Field** `ASSBJBNS` matches the **Filter Pattern** `BPXAS`. The output is then piped into `jq` to **Format** it.

###### Shell Script
```shell
cbxp -i assb -f assb.assbjbni=IBMUSER ascb | jq
```
