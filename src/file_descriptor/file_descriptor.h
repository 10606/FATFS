#ifndef FILE_DESCRIPTOR_H
#define FILE_DESCRIPTOR_H

#include "../structures/structure_.h"

void copy_file_descriptor (file_descriptor * dst, file_descriptor * src);
void copy_file_descriptor_seek_0 (file_descriptor * dst, file_descriptor * src);
char eq_file_descriptor (file_descriptor * a, file_descriptor * b);
uint32_t current_position (file_descriptor * fd);

#endif

