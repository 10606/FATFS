#ifndef READ_FILE_INFO_H
#define READ_FILE_INFO_H

#include <stdint.h>
#include "../structures/structure_.h"

uint32_t read_dir (file_descriptor * fd, file_descriptor * dst,  char * file_name);
uint32_t read_file_info (uint8_t * file_info, file_descriptor * fd, char * file_name);

extern const uint32_t max_size;
extern const uint32_t end_of_dir;
extern const uint32_t not_short_file_name;

#endif

