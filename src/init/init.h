#ifndef INIT_H
#define INIT_H

#include <stdint.h>
#include "../structures/structure_.h"

uint32_t init_fatfs 
(
    FAT_info_t * FAT_info, 
    uint32_t _sector_size,
    uint32_t _start_partition_sector, 
    uint32_t (* _read_sector) (uint32_t sector_number, void * buffer)
);

#endif

