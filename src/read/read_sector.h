#ifndef READ_SECTOR
#define READ_SECTOR

#include <stdint.h>

extern uint32_t start_partition_sector;
uint32_t read_sector (uint32_t sector_number, void * buffer);

#endif
