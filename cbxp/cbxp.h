#ifndef __CBXP_H_
#define __CBXP_H_

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

typedef struct {
  char* result_json;
  int result_json_length;
  int return_code;
} cbxp_result_t;

cbxp_result_t* cbxp(const char* control_block, const char* includes_string,
                    const char* filters_string, bool debug);

void cbxp_free(cbxp_result_t* cbxp_result, bool debug);

#ifdef __cplusplus
}
#endif

#endif
