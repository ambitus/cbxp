#ifndef __CBXP_H_
#define __CBXP_H_

#include "cbxp_result.h"

#ifdef __cplusplus
extern "C" {
#endif

cbxp_result_t* cbxp(const char* control_block, const char* includes_string,
                    const char* filters_string, bool debug);

void cbxp_free(cbxp_result_t* cbxp_result);

#ifdef __cplusplus
}
#endif

#pragma export(cbxp)
#pragma export(cbxp_free)

#endif
