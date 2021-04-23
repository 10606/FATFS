#ifndef READ_H
#define READ_H

#include <stdint.h>
#include "../file_descriptor/file_descriptor.h"

uint32_t f_read
(
    file_descriptor * file, 
    void * buffer, 
    uint32_t bytes, 
    uint32_t * bytes_read
);

uint32_t f_read_all
(
    file_descriptor * file, 
    void * buffer, 
    uint32_t bytes, 
    uint32_t * bytes_read
);

uint32_t f_read_all_fixed
(
    file_descriptor * file, 
    void * buffer, 
    uint32_t bytes, 
    uint32_t * bytes_read
);

uint32_t f_seek
(
    file_descriptor * file, 
    uint32_t bytes
);

#endif
