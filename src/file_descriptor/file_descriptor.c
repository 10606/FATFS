#include "../structures/structure_.h"
#include "file_descriptor.h"
#include <string.h>

void copy_file_descriptor (file_descriptor * dst, file_descriptor * src)
{
    dst->size = src->size; //in bytes
    dst->start_sector = src->start_sector;

    dst->current_sector = src->current_sector; //not readed
    dst->current_offset_in_sector = src->current_offset_in_sector; //not readed
    dst->sectors_read = src->sectors_read; //count of readed sectors to compare with size

    dst->is_dir = src->is_dir;
    memcpy(dst->buffer, src->buffer, max_sector_size);
}

void copy_file_descriptor_seek_0 (file_descriptor * dst, file_descriptor * src)
{
    dst->size = src->size; //in bytes
    dst->start_sector = src->start_sector;

    dst->current_sector = dst->start_sector; //not readed
    dst->current_offset_in_sector = 0; //not readed
    dst->sectors_read = 0; //count of readed sectors to compare with size

    dst->is_dir = src->is_dir;
}

char eq_file_descriptor (file_descriptor * a, file_descriptor * b)
{
    return a->start_sector == b->start_sector;
}

uint32_t current_position (file_descriptor * fd)
{
    if (fd->current_sector == 0)
        return fd->size;
    return (fd->sectors_read) * global_info.sector_size + 
        fd->current_offset_in_sector;
}

