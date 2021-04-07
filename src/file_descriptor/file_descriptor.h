#ifndef FILE_DESCRIPTOR_H
#define FILE_DESCRIPTOR_H

#include "../structures/structure_.h"

void copy_file_descriptor (file_descriptor * dst, file_descriptor const * src);
void copy_file_descriptor_seek_0 (file_descriptor * dst, file_descriptor const * src);
char eq_file_descriptor (file_descriptor const * a, file_descriptor const * b);
uint32_t current_position (file_descriptor * fd);

void init_fake_file_descriptor (file_descriptor * fd); // start_sector = 0, f_read return end_of_file, f_seek return 0
char is_fake_file_descriptor (file_descriptor * fd);

#endif

