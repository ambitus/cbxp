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

cbxp_result_t* cbxp_extract(const char* control_block_name,
                            const unsigned int control_block_name_length,
                            const char* includes,
                            const unsigned int includes_length,
                            const char* filters,
                            const unsigned int filters_length, bool debug);

cbxp_result_t* cbxp_format(const char* control_block_name,
                           const unsigned int control_block_name_length,
                           const void* bytes_buffer,
                           const unsigned int buffer_length, bool debug);

void cbxp_free(cbxp_result_t* cbxp_result, bool debug);

#ifdef __cplusplus
}
#endif

#endif
