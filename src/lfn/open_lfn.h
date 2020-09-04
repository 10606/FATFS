#ifndef OPEN_LFN_H
#define OPEN_LFN_H

#include <stdint.h>
#include "../structures/structure.h"
#include "../structures/structure_.h"

extern uint32_t const not_found;
extern uint32_t const path_consist_not_dir;

uint32_t open_lfn
(
    file_descriptor * file,
    char const * const * long_path, //[path_len][len_i]
    char (* path)[12], //[path_len][11]
    uint32_t path_len
);

#endif

