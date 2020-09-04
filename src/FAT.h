#ifndef FAT_H
#define FAT_H

#include "read/read_sector.h"
#include "read/read.h"
#include "init/init.h"
#include "load_FAT/load_FAT.h"
#include "open/open.h"
#include "read_file_info/read_file_info.h"
#include "structures/structure_.h"
#include "file_descriptor/file_descriptor.h"

extern uint32_t start_partition_sector;
uint32_t read_sector (uint32_t sector_number, void * buffer);

#endif

