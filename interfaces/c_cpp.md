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
> _The **C/C++** interface for CBXP downloaded from [GitHub](https://github.com/ambitus/cbxp/releases)._

&nbsp;

{: .note }
> _The **CBXP PAX** provides a **Static Library** for CBXP at `/lib/libcbxp.a` and a **C Header** for CBXP at `/include/cbxp.h`. To compile code that uses the **C/C++** interface for CBXP, **Include** `/include/cbxp.h` at **Compile Time** and **Link** with `/lib/libcbxp.a`._

&nbsp;

{: .note }
> _When compiling and linking **C/C++** code with `/lib/libcbxp.a`, it is recommended to use [IBM Open XL C/C++ 2.2](https://www.ibm.com/docs/en/open-xl-c-cpp-zos/2.2.0) or newer. See [Binary Compatibility](https://www.ibm.com/docs/en/open-xl-c-cpp-zos/2.2.0?topic=guide-binary-compatibility) for more details._

&nbsp;

## `cbxp()`

```c
cbxp_result_t* cbxp(const char* control_block, const char* includes_string,
                    const char* filters_string, bool debug);
```

### 📄 Description

&nbsp;

{: .note }
> _`includes_string` and `filters_string` are **Optional Parameters** that can be excluded by specifying `NULL` or `nullptr`._

&nbsp;

Extract **Control Blocks** from **Live Memory** *(storage)* and post-process them into **JSON**.

### 📥 Parameters

* `control_block` <br>
  A **NULL-Terminated ISO8859-1 Encoded String** that contains name of the **Control Block** to extract.

* `includes_string` <br>
  A **NULL-Terminated ISO8859-1 Encoded String** that contains a **Comma Delimited List** of [Include Patterns](../../include_patterns) that describe **Additional Control Blocks** to include that are accessible from the **Root Control Block** being extracted.

* `filters_string` <br>
  A **NULL-Terminated ISO8859-1 Encoded String** that contains a **Comma Delimited List** of [Filters](../../filters) that are used to filter the entries returned in **Repeated** control block data.

* `debug` <br>
  A **Boolean** that if set to `true` indicates that **Debug Messages** should be printed. If set to `false`, no **Debug Messages** will be printed.

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

Free all **Dynamically Allocated Memory** associated with a [`cbxp_result_t`](#cbxp_result_t) pointer returned by `cbxp()`.

### 📥 Parameters

* `cbxp_result` <br>
  A pointer to a [`cbxp_result_t`](#cbxp_result_t) **C Struct**.

* `debug` <br>
  A **Boolean** that if set to `true` indicates that **Debug Messages** should be printed. If set to `false`, no **Debug Messages** will be printed.

### 📤 Returns
* `void` <br>
  This function does not return anything.

## `cbxp_result_t`

```c
typedef struct {
  char* result_json;
  int result_json_length;
  int return_code;
} cbxp_result_t;
```

### 📄 Description

The **C Struct** that is used to store all **Result Information** returned by CBXP.

### 📋 Fields

* `result_json` <br>
  When the call to `cbxp()` is **Successful**, this field contains pointer to a **NULL-Terminated ISO8859-1 Encoded JSON String**. When the call to `cbxp()` is **Unsucessful**, this field contains a **NULL Pointer**.

* `result_json_length` <br>
  When the call to `cbxp()` is **Successful**, this field contains the length of the **NULL-Terminated ISO8859-1 Encoded JSON String** stored in the `result_json` field. When the call to `cbxp()` is **Unsuccessful**, this field contains `0`.

* `return_code` <br>
  This field will be set by `cbxp()` to one of the following values:
  * ✅ `0`<br>
    The call to `cbxp()` was **Successful**.
  * ❌ `1`<br>
    The call to `cbxp()` was **Unsuccessful** due to an **Unknown Control Block** being specified with the `control_block` parameter.
  * ❌ `2`<br>
    The call to `cbxp()` was **Unsuccessful** due to a bad [Include Pattern](../../include_patterns) being specified within the `includes_string` parameter.
  * ❌ `3`<br>
    The call to `cbxp()` was **Unsuccessful** due to a bad [Filter](../../filters) being specified within the `filters_string` parameter.

## 💻 Examples

&nbsp;

{: .note }
> _The following examples assume that the **CBXP PAX** is extracted in the **Current Working Directory**._
> <br><br>
> ```
> .
> |-- cbxp-{{ site.cbxp_version }}
> |   |-- LICENSE
> |   |-- NOTICES
> |   |-- bin
> |   |   `-- cbxp
> |   |-- include
> |   |   `-- cbxp.h
> |   `-- lib
> |       `-- libcbxp.a
> |-- cbxp-{{ site.cbxp_version }}.pax.Z
> |-- main.c
> |-- main.cpp
> ```

&nbsp;

The following **C** example extracts the [PSA](https://www.ibm.com/docs/en/zos/latest?topic=rqe-psa-information) control block and prints the returned **JSON String**.

###### C
```c
#include <cbxp.h>
#include <stdbool.h>
#include <stdio.h>

int main() {
  // Call CBXP
  puts("Calling CBXP from C...");
  cbxp_result_t* cbxp_result = cbxp("psa", NULL, NULL, false);

  // Make sure call was successful
  if (cbxp_result->return_code != 0) {
    fprintf(stderr, "Error: CBXP return with RC=%d\n", cbxp_result->return_code);
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
ibm-clang64 -c -fzos-le-char-mode=ascii -I./cbxp-{{ site.cbxp_version }}/include -o main.o main.c
# Link
ibm-clang++64 -fzos-le-char-mode=ascii -o main ./cbxp-{{ site.cbxp_version }}/lib/libcbxp.a main.o
# Run
./main
```

&nbsp;

The following **C++** example extracts the [PSA](https://www.ibm.com/docs/en/zos/latest?topic=rqe-psa-information) control block and prints the returned **JSON String**.

###### C++
```cpp
#include <iostream>

#include <cbxp.h>

int main() {
  // Call CBXP
  std::cout << "Calling CBXP from C++..." << std::endl;
  cbxp_result_t * cbxp_result = cbxp("psa", nullptr, nullptr, false);

  // Make sure call was successful
  if (cbxp_result->return_code != 0) {
    std::cerr << "Error: CBXP returned with RC=" << cbxp_result->return_code << std::endl;
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
ibm-clang++64 -c -fzos-le-char-mode=ascii -I./cbxp-{{ site.cbxp_version }}/include -o main.o main.cpp
# Link
ibm-clang++64 -fzos-le-char-mode=ascii -o main ./cbxp-{{ site.cbxp_version }}/lib/libcbxp.a main.o
# Run
./main
```