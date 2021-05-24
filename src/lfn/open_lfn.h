#ifndef OPEN_LFN_H
#define OPEN_LFN_H

#include <stdint.h>
#include "../structures/structure.h"
#include "../file_descriptor/file_descriptor.h"

namespace err
{
    extern uint32_t const not_found;
    extern uint32_t const path_consist_not_dir;
    extern uint32_t const not_long_file_name;
}

uint32_t open_lfn
(
    FAT_info_t * FAT_info, 
    file_descriptor * file,
    uint16_t const * const * long_path, //[path_len][len_i]
    char (* path)[12], //[path_len][12]
    uint32_t path_len
);

#endif

