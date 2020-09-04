#ifndef LOAD_FAT_H
#define LOAD_FAT_H

#include <stdint.h>
#include "../structures/structure_.h"

uint32_t next_sector (uint32_t sector, uint32_t * answer);

//TODO enum
extern const uint32_t invalid_cluster;
extern const uint32_t eof_cluster;
extern const uint32_t eof_file;

#endif
