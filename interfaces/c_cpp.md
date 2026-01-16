---
layout: default
parent: Interfaces
nav_order: 3
---

# C/C++

The following library interface is provided to facilitate exploitation of CBXP by C/C++ callers.
{: .fs-6 .fw-300 }

&nbsp;

{: .note }
> _The **Library** interface for CBXP downloaded from [GitHub](https://github.com/ambitus/cbxp/releases)._

&nbsp;

{: .note }
> _Wherever you **Install**/**Extract** the **CBXP Pax File**, make sure to add the path to the `lib` directory where the `libcbxp.so` library resides to `LIBPATH` in `/etc/profile` (global) or `~/.profile`/`~/.bashrc` (local/individual)._

&nbsp;

{: .warning }
> _This interface is [Non-Rentrent](https://en.wikipedia.org/wiki/Reentrancy_(computing)), meaning that it is **NOT** [Thread Safe](https://en.wikipedia.org/wiki/Thread_safety). If multiple threads may be calling this interface simultaneously, you should use a **Mutex** or some other serialization mechanisim to avoid [Race Conditions](https://en.wikipedia.org/wiki/Race_condition)._

## `cbxp()`

```c
typedef const cbxp_result_t* (*cbxp_t)(const char* control_block_name,
                                       const char* includes_string,
                                       const char* filters_string, bool debug);
```

### 📄 Description

Extract **Control Blocks** from **Live Memory** *(storage)*, post-process them into **JSON**, and return a pointer to a **C Struct** containing a pointer to the **Result JSON String**, the **Length** of the **Result JSON String**, and a **Return Code**.

### 📥 Parameters

* `control_block_name` <br>
  The name of the **Control Block** to extract.

* `includes_string` <br>
  A **Comma Delimited List** of [Include Patterns](../../include_patterns) that describe **Additional Control Blocks** to include that are accessible from the **Root Control Block** being extracted.

* `filters_string` <br>
  A **Comman Delimited List** of [Filters](../../filters) that are used to filter the entries returned in **Repeated** control block data.

* `debug` <br>
  A **Boolean** that if set to `true` indicates that **Debug Messages** should be printed. If set to `false`, no **Debug Messages** will be printed.

### 📤 Returns
* `const cbxp_result_t*` <br>
  A **C Struct** that contains a pointer to the **Result JSON String**, the **Length** of the **Result JSON String**, and a **Return Code**.

### 💻 Examples

The following example extracts the [PSA](https://www.ibm.com/docs/en/zos/latest?topic=rqe-psa-information) control block and prints **Debug Messages**.

###### C
```c
#define _UNIX03_SOURCE

#include <dlfcn.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct {
  char* result_json;
  int result_json_length;
  int return_code;
} cbxp_result_t;

typedef const cbxp_result_t* (*cbxp_t)(const char* control_block_name,
                                       const char* includes_string,
                                       const char* filters_string, bool debug);

int main() {
  // Load 'libcbxp.so'
  void* lib_handle = dlopen("libcbxp.so", RTLD_NOW);
  if (lib_handle == NULL) {
    perror("Unable to load 'libcbxp.so'.");
    return -1;
  }

  // Resolve symbol 'cbxp()'
  cbxp_t cbxp = (cbxp_t)dlsym(lib_handle, "cbxp");
  if (cbxp == NULL) {
    perror("Unable to resolve symbol 'cbxp()'.");
    if (dlclose(lib_handle) != 0) {
      const char* error_string = dlerror();
      if (error_string != NULL) {
        puts(error_string);
      }
    }
    return -1;
  }

  // Call 'cbxp()'
  const cbxp_result_t* cbxp_result = cbxp("psa", "", "", true);

  // Print result json string
  puts(cbxp_result->result_json);

  // Close 'libcbxp.so'
  if (dlclose(lib_handle) != 0) {
    const char* error_string = dlerror();
    if (error_string != NULL) {
      puts(error_string);
    }
    return -1;
  }

  return 0;
}
```

###### Shell
```shell
ibm-clang64 -fzos-le-char-mode=ascii -o main main.c && ./main
```
