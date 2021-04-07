#include "../structures/structure_.h"
#include "file_descriptor.h"
#include <string.h>

void copy_file_descriptor (file_descriptor * dst, file_descriptor const * src)
{
    dst->FAT_info = src->FAT_info;
    dst->size = src->size; //in bytes
    dst->start_sector = src->start_sector;

    dst->current_sector = src->current_sector; //not readed
    dst->current_offset_in_sector = src->current_offset_in_sector; //not readed
    dst->sectors_read = src->sectors_read; //count of readed sectors to compare with size

    dst->is_dir = src->is_dir;
    memcpy(dst->buffer, src->buffer, max_sector_size);
}

void copy_file_descriptor_seek_0 (file_descriptor * dst, file_descriptor const * src)
{
    dst->FAT_info = src->FAT_info;
    dst->size = src->size; //in bytes
    dst->start_sector = src->start_sector;

    dst->current_sector = dst->start_sector; //not readed
    dst->current_offset_in_sector = 0; //not readed
    dst->sectors_read = 0; //count of readed sectors to compare with size

    dst->is_dir = src->is_dir;
}

char eq_file_descriptor (file_descriptor const * a, file_descriptor const * b)
{
    return (a->FAT_info == b->FAT_info) && (a->start_sector == b->start_sector);
}

uint32_t current_position (file_descriptor * fd)
{
    if (fd->start_sector == 0)
        return 0;
    if (fd->current_sector == 0)
        return fd->size;
    return (fd->sectors_read) * fd->FAT_info->global_info.sector_size + 
        fd->current_offset_in_sector;
}

void init_fake_file_descriptor (file_descriptor * fd)
{
    fd->size = 0;
    fd->start_sector = 0;
    
    fd->current_sector = 0;
    fd->current_offset_in_sector = 0;
    fd->sectors_read = 0;
 
    fd->FAT_info = 0;
    fd->is_dir = 0;
}

char is_fake_file_descriptor (file_descriptor * fd)
{
    return (fd->start_sector == 0) && (fd->current_sector == 0);
}

