---
layout: default
parent: Interfaces
---

# Shell

The following shell interface is provided to facilitate exploitation of CBXP by shell callers.
{: .fs-6 .fw-300 }

&nbsp;

{: .note }
> _The **Shell Interface** for CBXP downloaded from [GitHub](https://github.com/ambitus/cbxp/releases)._

&nbsp;

{: .note }
> _Wherever you **Install**/**Extract** the **CBXP Pax File**, make sure to add the path to the `bin` directory where the `cbxp` executable resides to `PATH` in `/etc/profile` (global) or `~/.profile`/`~/.bashrc` (local/individual)._

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


The followng example extracts all [ASCB](https://www.ibm.com/docs/en/zos/latest?topic=iar-ascb-information) control blocks and pipes the output into `jq` to **Format** the JSON output.

###### Shell Script
```shell
cbxp ascb | jq
```
