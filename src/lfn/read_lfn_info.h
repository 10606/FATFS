#ifndef READ_LFN_INFO
#define READ_LFN_INFO

#include <stdint.h>
#include "../structures/structure.h"
#include "../file_descriptor/file_descriptor.h"

uint32_t read_lfn_info (long_file_name * lfn, char * long_name);
uint32_t read_dir_lfn 
(
    file_descriptor * fd, 
    file_descriptor * dst,  
    char * file_name, 
    char * long_name
);

#endif

