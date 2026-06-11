---
layout: default
parent: Interfaces
nav_order: 2
---

# CLI

The following CLI interface is provided to facilitate exploitation of CBXP by shell callers.
{: .fs-6 .fw-300 }

&nbsp;

{: .note }
> _The **CLI** interface for CBXP can be installed from [zopen community](https://github.com/zopencommunity/cbxpport) using the [`zopen`](https://zopen.community/Guides/ThePackageManager) package manager._
> 
> <br>
>
> ```shell
> zopen install cbxp
> ```
> _The **CLI** interface for CBXP may also optionally be downloaded from [GitHub](https://github.com/ambitus/cbxp/releases)._

&nbsp;

## `cbxp extract`

```
cbxp extract [flags] <control block>
```

### 📄 Description

Extract and format **Control Block Data** from **Live Memory**.

### 🚩 Flags

* `-i`, `--include <pattern>` <br>
  **Include** control blocks that are **Accessible** from the **Root Control Block** being extracted using an [Include Pattern](../../include_patterns).

* `-f`, `--filter` <br>
  **Filter** repeated control block data using a [Filter](../../filters).

### 🌐 Global Flags

* `-d`, `--debug` <br>
  Print **Debug Messages**.

* `-h`, `--help` <br>
  Display **Usage Information**.

### 💻 Examples

The following example extracts the [PSA](https://www.ibm.com/docs/en/zos/latest?topic=rqe-psa-information) control block from **Live Memory** and prints **Debug Messages**.

###### Shell Script
```shell
cbxp extract -d psa
```

&nbsp;

The following example extracts the [CVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-cvt-information) control block from **Live Memory**, includes the [ECVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-ecvt-information) control block and the [ASVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-asvt-information) control block, and includes all **Known Control Blocks** that are pointed to directly by the [ASVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-asvt-information) control block.

###### Shell Script
```shell
cbxp extract -i ecvt -i 'asvt.*' cvt
```

&nbsp;

The following example extracts all [ASSB](https://www.ibm.com/docs/en/zos/latest?topic=iar-assb-information) control blocks from **Live Memory** where both the **Control Block Field** `ASSBJBNI` matches the **Filter Value** `IBMUSER` and the **Control Block Field** `ASSBJBNS` matches the **Filter Value** `BPXAS`. The **JSON** output is then piped into `jq` to **Format** it.

###### Shell Script
```shell
cbxp extract -f assb.assbjbni=IBMUSER assb | jq
```

## `cbxp format`
```
cbxp format [flags] <control block>
```

### 📄 Description

Format **Control Block Data** from a **File/Pipe**.

### 🚩 Flags

* `-F`, `--file <path>` <br>
  Format control block data from a specified **File** or **Pipe**.

* `-o`, `--offset <offset>` <br>
  Specify an **Offset** into the provided data to start formatting at.

### 🌐 Global Flags

* `-d`, `--debug` <br>
  Print **Debug Messages**.

* `-h`, `--help` <br>
  Display **Usage Information**.

### 💻 Examples

The following example formats [CVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-cvt-information) control block data from a **File** and prints **Debug Messages**.

###### Shell Script
```shell
cbxp format -F cvt.bin -d cvt
```

&nbsp;

The following example formats [CVT](https://www.ibm.com/docs/en/zos/latest?topic=iar-cvt-information) control block data from a **Pipe**.

###### Shell Script
```shell
cat cvt.bin | cbxp format cvt
```

&nbsp;

The following example formats [ASCB](https://www.ibm.com/docs/en/zos/latest?topic=iar-ascb-information) control block data from a **File** at an offset of **0x40** bytes.

###### Shell Script
```shell
cbxp format -F ascboffset.bin -o 0x40 ascb
```

&nbsp;

The following example formats [ASCB](https://www.ibm.com/docs/en/zos/latest?topic=iar-ascb-information) control block data from a **File** at an offset of **64** bytes.

###### Shell Script
```shell
cbxp format -F ascboffset.bin -o 64 ascb
```
