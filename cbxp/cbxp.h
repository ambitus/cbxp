#ifndef __CBXP_H_
#define __CBXP_H_

#include "cbxp_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
This is the main interface to CBXP.

The following pointers must be freed after calling this interface to
avoid memory leaks:

  result.result_json

*/
cbxp_result_t* cbxp(const char* control_block_name, const char* includes_string,
                    bool debug);

#ifdef __cplusplus
}
#endif

#pragma export(cbxp)

#endif
