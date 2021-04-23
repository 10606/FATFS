#ifndef READ_FILE_INFO_H
#define READ_FILE_INFO_H

#include <stdint.h>
#include "../structures/structure.h"
#include "../file_descriptor/file_descriptor.h"

uint32_t read_dir 
(
    file_descriptor * fd, 
    file_descriptor * dst,  
    char * file_name
);

uint32_t read_file_info 
(
    FAT_info_t * FAT_info, 
    short_file_name * sfn, 
    file_descriptor * fd, 
    char * file_name
);

namespace err
{
    extern const uint32_t end_of_dir;
    extern const uint32_t not_short_file_name;
};

#endif

