#ifndef __CBXP_H_
#define __CBXP_H_

#ifdef __cplusplus
#include <cstddef>
extern "C" {
#else
#include <stdbool.h>
#include <stddef.h>
#endif

typedef struct {
  char* result_json;
  int result_json_length;
  int return_code;
} cbxp_result_t;

cbxp_result_t* cbxp_extract(const char* control_block_name,
                            const size_t control_block_name_length,
                            const char* includes, const size_t includes_length,
                            const char* filters, const size_t filters_length,
                            bool debug);

cbxp_result_t* cbxp_format(const char* control_block_name,
                           const size_t control_block_name_length,
                           const void* data, const size_t data_length,
                           bool debug);

void cbxp_free(cbxp_result_t* cbxp_result, bool debug);

#ifdef __cplusplus
}
#endif

#endif
