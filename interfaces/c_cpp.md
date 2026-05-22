---
layout: default
parent: Interfaces
nav_order: 3
---

# C/C++

The following C/C++ interface is provided to facilitate exploitation of CBXP by C/C++ callers.
{: .fs-6 .fw-300 }

&nbsp;

{: .note }
> _The **C/C++** interface for CBXP can be installed from [zopen community](https://github.com/zopencommunity/cbxpport) using the [`zopen`](https://zopen.community/Guides/ThePackageManager) package manager._
> 
> <br>
>
> ```shell
> zopen install cbxp
> ```
> _The **C/C++** interface for CBXP may also optionally be downloaded from [GitHub](https://github.com/ambitus/cbxp/releases)._

&nbsp;

{: .note }
> _A `libcbxp.a` **Static Library** and `cbxp.h` **C Header** are provided for use by C/C++ callers. To compile code that uses the **C/C++** interface for CBXP, **Include** `cbxp.h` at **Compile Time** and **Link** with `libcbxp.a`._

&nbsp;

{: .note }
> _When compiling and linking **C/C++** code with `libcbxp.a`, it is recommended to use [IBM Open XL C/C++ 2.2](https://www.ibm.com/docs/en/open-xl-c-cpp-zos/2.2.0) or newer. See [Binary Compatibility](https://www.ibm.com/docs/en/open-xl-c-cpp-zos/2.2.0?topic=guide-binary-compatibility) for more details._


## `cbxp_extract()`

```c
cbxp_result_t* cbxp_extract(const char* control_block_name,
                            const size_t control_block_name_length,
                            const char* includes, const size_t includes_length,
                            const char* filters, const size_t filters_length,
                            bool debug);
```

### 📄 Description

&nbsp;

{: .note }
> _`includes` and `filters` are **Optional Parameters** that can be excluded by specifying `NULL` or `nullptr`._

&nbsp;

Extract and format **Control Block Data** from **Live Memory**.

### 📥 Parameters

* `control_block_name` <br>
  A pointer to a **NULL-Terminated ISO8859-1 Encoded String** that contains name of the **Control Block** to extract.

* `control_block_name_length` <br>
  The length of `control_block_name`.

* `includes` <br>
  A pointer to a **NULL-Terminated ISO8859-1 Encoded String** that contains a **Comma Delimited List** of [Include Patterns](../../include_patterns) that describe **Additional Control Blocks** to include that are accessible from the **Root Control Block** being extracted.

* `includes_length` <br>
  The length of `includes`.

* `filters` <br>
  A pointer to a **NULL-Terminated ISO8859-1 Encoded String** that contains a **Comma Delimited List** of [Filters](../../filters) that are used to filter the entries returned in **Repeated** control block data.

* `filters_length` <br>
  The length of `filters`.

* `debug` <br>
  A **Boolean** that if set to `true`, indicates that **Debug Messages** should be printed. If set to `false`, no **Debug Messages** will be printed.

### 📤 Returns
* `cbxp_result_t*` <br>
  A pointer to a [`cbxp_result_t`](#cbxp_result_t) **C Struct**.

## `cbxp_format()`

```c
cbxp_result_t* cbxp_format(const char* control_block_name,
                           const size_t control_block_name_length,
                           const void* p_data, const size_t data_length,
                           bool debug);
```

### 📄 Description

Format **Caller-Provided Control Block Data**.

### 📥 Parameters

* `control_block_name` <br>
  A pointer to a **NULL-Terminated ISO8859-1 Encoded String** that contains name of the **Control Block** to format.

* `control_block_name_length` <br>
  The length of `control_block_name`.

* `data` <br>
  A pointer to a **Buffer** containing **Raw Control Block Data** to format.

* `data_length` <br>
  The length of `data`.

* `debug` <br>
  A **Boolean** that if set to `true`, indicates that **Debug Messages** should be printed. If set to `false`, no **Debug Messages** will be printed.

### 📤 Returns
* `cbxp_result_t*` <br>
  A pointer to a [`cbxp_result_t`](#cbxp_result_t) **C Struct**.

## `cbxp_free()`

```c
void cbxp_free(cbxp_result_t* cbxp_result, bool debug);
```

### 📄 Description

&nbsp;

{: .warning}
> _The [`cbxp_result_t`](#cbxp_result_t) pointer and the `result_json` pointer within the [`cbxp_result_t`](#cbxp_result_t) **C Struct** it points to are no longer valid after calling `cbxp_free()`. Using these pointers after calling `cbxp_free()` will result in **Undefined Behavior** since `cbxp_free()` frees the memory that both of these pointer point to._

&nbsp;

Free all **Dynamically Allocated Memory** associated with a [`cbxp_result_t`](#cbxp_result_t) pointer returned by `cbxp_extract()` / `cbxp_format()`.

### 📥 Parameters

* `cbxp_result` <br>
  A pointer to a [`cbxp_result_t`](#cbxp_result_t) **C Struct**.

* `debug` <br>
  A **Boolean** that if set to `true`, indicates that **Debug Messages** should be printed. If set to `false`, no **Debug Messages** will be printed.

### 📤 Returns
* `void` <br>
  This function does not return anything.

## `cbxp_result_t`

```c
typedef struct {
  char* result_json;
  int result_json_length;
  unsigned int return_code;
} cbxp_result_t;
```

### 📄 Description

The **C Struct** that is used to store all **Result Information** returned by [`cbxp_extract()`](#cbxp_extract) and [`cbxp_format()`](#cbxp_format).

### 📋 Fields

* `result_json` <br>
  When the call to `cbxp_extract()` / `cbxp_format()` is **Successful**, this field contains pointer to a **NULL-Terminated ISO8859-1 Encoded JSON String**. When the call to `cbxp_extract()` / `cbxp_format()` is **Unsucessful**, this field contains a **NULL Pointer**.

* `result_json_length` <br>
  When the call to `cbxp_extract()` / `cbxp_format()` is **Successful**, this field contains the length of the **NULL-Terminated ISO8859-1 Encoded JSON String** stored in the `result_json` field. When the call to `cbxp_extract()` / `cbxp_format()` is **Unsuccessful**, this field contains `0`.

* `return_code` <br>
  This field will be set by `cbxp_extract()` / `cbxp_format()` to one of the following values:
  * ✅ `0`<br>
    The call to `cbxp_extract()` / `cbxp_format()` was **Successful**.
  * ❌ `1`<br>
    The call to `cbxp_extract()` / `cbxp_format()` was **Unsuccessful** due to an **Unknown Control Block** being specified with the `control_block_name` parameter.
  * ❌ `2`<br>
    The call to `cbxp_extract()` was **Unsuccessful** due to a bad [Include Pattern](../../include_patterns) being specified within the `includes` parameter.
  * ❌ `3`<br>
    The call to `cbxp_extract()` was **Unsuccessful** due to a bad [Filter](../../filters) being specified within the `filters` parameter.
  * ❌ `4`<br>
    The call to `cbxp_format()` was **Unsuccessful** due to the data provided in the `data` parameter being too small to format the specified control block from.
  * ❌ `5`<br>
    The call to `cbxp_format()` was **Unsuccessful** due to the `data` parameter being a **NULL** pointer.

## 💻 Examples

&nbsp;

{: .note }
> _The following examples assumes that CBXP was installed from [zopen community](https://github.com/zopencommunity/cbxpport) using the [`zopen`](https://zopen.community/Guides/ThePackageManager) package manager._
> <br><br>
> ```
> /usr/local/zopen/usr/local/zopen/cbxp/cbxp/
> |-- LICENSE
> |-- NOTICES
> |-- README.md
> |-- altbin
> |-- bin
> |   `-- cbxp
> |-- include
> |   `-- cbxp.h
> |-- install_test.sh
> |-- lib
> |   `-- libcbxp.a
> |-- metadata.json
> |-- setup.sh
> |-- share
> |   `-- altman
> |       `-- man1
> `-- test.status
> 
> 8 directories, 10 files
> ```

&nbsp;

The following **C** example uses [`cbxp_extract()`](#cbxp_extract) to extract and format the [PSA](https://www.ibm.com/docs/en/zos/latest?topic=rqe-psa-information) control block from **Live Memory**, and prints the returned **JSON String**.

###### C
```c
#include <cbxp.h>
#include <stdio.h>

int main() {
  // Call 'cbxp_extract()'
  puts("Calling 'cbxp_extract()' from C...");
  cbxp_result_t* cbxp_result = cbxp_extract("psa", 3, NULL, 0, NULL, 0, false);

  // Make sure call was successful
  if (cbxp_result->return_code != 0) {
    fprintf(stderr, "Error: 'cbxp_extract()' returned with rc=%d\n", cbxp_result->return_code);
  // Print Result JSON
  } else {
    puts(cbxp_result->result_json);
  }

  // Cleanup
  cbxp_free(cbxp_result, false);

  return 0;
}
```

###### Shell
```shell
# Compile
ibm-clang64 -c -fzos-le-char-mode=ascii \
            -I /usr/local/zopen/usr/local/zopen/cbxp/cbxp/include \
            -o extract.o extract.c
# Link
ibm-clang++64 -fzos-le-char-mode=ascii -o extract \
              /usr/local/zopen/usr/local/zopen/cbxp/cbxp/lib/libcbxp.a extract.o
# Run
./extract
```

&nbsp;

The following **C++** example uses [`cbxp_extract()`](#cbxp_extract) to extract and format the [PSA](https://www.ibm.com/docs/en/zos/latest?topic=rqe-psa-information) control block from **Live Memory**, and prints the returned **JSON String**.

###### C++
```cpp
#include <iostream>

#include <cbxp.h>

int main() {
  // Call 'cbxp_extract()'
  std::cout << "Calling 'cbxp_extract()' from C++..." << std::endl;
  cbxp_result_t * cbxp_result = cbxp_extract("psa", 3, nullptr, 0, nullptr, 0, false);

  // Make sure call was successful
  if (cbxp_result->return_code != 0) {
    std::cerr << "Error: 'cbxp_extract()' returned with rc=" 
              << cbxp_result->return_code << std::endl;
  // Print result JSON
  } else {
    std::cout << cbxp_result->result_json << std::endl;
  }

  // Cleanup
  cbxp_free(cbxp_result, false);

  return 0;
}
```

###### Shell
```shell
# Compile
ibm-clang++64 -c -fzos-le-char-mode=ascii \
              -I /usr/local/zopen/usr/local/zopen/cbxp/cbxp/include \
              -o extract.o extract.cpp
# Link
ibm-clang++64 -fzos-le-char-mode=ascii -o extract \
              /usr/local/zopen/usr/local/zopen/cbxp/cbxp/lib/libcbxp.a extract.o
# Run
./extract
```

&nbsp;

The following **C** example uses [`cbxp_format()`](#cbxp_format) to format **Caller-Provided** [ASCB](https://www.ibm.com/docs/en/zos/latest?topic=iar-ascb-information) control block data and prints the returned **JSON String**.

###### C
```c
#include <cbxp.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  // Open ascb.bin
  FILE *fp = fopen("ascb.bin", "rb");
  if (fp == NULL) {
    fprintf(stderr, "Error: ascb.bin not found\n");
    return -1;
  }

  // Get size of ascb.bin
  fseek(fp,0,SEEK_END);
  long size = ftell(fp);
  fseek(fp,0,SEEK_SET); 

  // Allocate memory to load control block data
  void* data = malloc(size);
  if (data == NULL) {
    fprintf(stderr, "Error: unable to allocate memory to load control block data\n");
    fclose(fp);
    return -1;
  }

  // Read control block data
  fread(data, sizeof(void), size, fp);

  // Close file
  fclose(fp);

  // Call 'cbxp_format()'
  puts("Calling 'cbxp_format()' from C...");
  cbxp_result_t* cbxp_result = cbxp_format("ascb", 4, data, size, false);

  // Make sure call was successful
  if (cbxp_result->return_code != 0) {
    fprintf(stderr, "Error: 'cbxp_format()' returned with rc=%d\n", cbxp_result->return_code);
  // Print Result JSON
  } else {
    puts(cbxp_result->result_json);
  }

  // Cleanup
  cbxp_free(cbxp_result, false);
  free(data);

  return 0;
}
```

###### Shell
```shell
# Compile
ibm-clang64 -c -fzos-le-char-mode=ascii \
            -I /usr/local/zopen/usr/local/zopen/cbxp/cbxp/include \
            -o format.o format.c
# Link
ibm-clang++64 -fzos-le-char-mode=ascii -o format \
              /usr/local/zopen/usr/local/zopen/cbxp/cbxp/lib/libcbxp.a format.o
# Run
./format
```

&nbsp;

The following **C++** example uses [`cbxp_format()`](#cbxp_format) to format **Caller-Provided** [ASCB](https://www.ibm.com/docs/en/zos/latest?topic=iar-ascb-information) control block data and prints the returned **JSON String**.

###### C++
```cpp
#include <fstream>
#include <iostream>

#include <cbxp.h>

int main() {
  // Open ascb.bin
  std::ifstream file("ascb.bin", std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    std::cerr << "Error: ascb.bin not found" << std::endl;
    return -1;
  }

  // Get size of ascb.bin
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  // Create vector to store control block data
  std::vector<char> data;
  data.resize(size);

  // Read control block data
  file.read(data.data(), size);

  // Close file
  file.close();

  // Call 'cbxp_format()'
  std::cout << "Calling 'cbxp_format()' from C++..." << std::endl;
  cbxp_result_t * cbxp_result = cbxp_format("ascb", 4,
                                            static_cast<void*>(data.data()),
                                            size, false);

  // Make sure call was successful
  if (cbxp_result->return_code != 0) {
    std::cerr << "Error: 'cbxp_format()' returned with rc=" 
              << cbxp_result->return_code << std::endl;
  // Print result JSON
  } else {
    std::cout << cbxp_result->result_json << std::endl;
  }

  // Cleanup
  cbxp_free(cbxp_result, false);

  return 0;
}
```

###### Shell
```shell
# Compile
ibm-clang++64 -c -fzos-le-char-mode=ascii \
              -I /usr/local/zopen/usr/local/zopen/cbxp/cbxp/include \
              -o format.o format.cpp
# Link
ibm-clang++64 -fzos-le-char-mode=ascii -o format \
              /usr/local/zopen/usr/local/zopen/cbxp/cbxp/lib/libcbxp.a format.o
# Run
./format
```