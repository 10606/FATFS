#ifndef OPEN_H
#define OPEN_H

#include "../fat_info/fat_info.h"
#include "../file_descriptor/file_descriptor.h"

#include <stdint.h>

namespace err
{
    extern uint32_t const not_found;
    extern uint32_t const path_consist_not_dir;
}

uint32_t open 
(
    FAT_info_t * FAT_info, 
    file_descriptor * file,
    char const (* const path)[12], //[path_len][11]
    uint32_t path_len
);

#endif

