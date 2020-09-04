#ifndef READ_LFN_INFO
#define READ_LFN_INFO

#include <stdint.h>
#include "../structures/structure.h"
#include "../structures/structure_.h"

extern const uint32_t not_long_file_name;
uint32_t read_lfn_info (uint8_t * file_info, char * long_file_name);
uint32_t read_dir_lfn (file_descriptor * fd, file_descriptor * dst,  char * file_name, char * long_name);

#endif

