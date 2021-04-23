#include "read.h"

#include <stdint.h>
#include <string.h>
#include "../fat_info/fat_info.h"

static uint32_t f_read_next_sector
(
    file_descriptor * file, 
    uint32_t bytes, 
    uint32_t * bytes_read
)
{
    uint32_t next, ret;
    [[unlikely]] if ((ret = file->FAT_info->next_sector(file->current_sector, next)))
    {
        [[likely]] if (ret == err::eof_cluster)
        {
            file->current_sector = 0;
            file->current_offset_in_sector = file->size % file->FAT_info->global_info.sector_size;
            *bytes_read = bytes;
            return 0;
        }
        else
        {
            *bytes_read = 0;
            return ret;
        }
    }
    file->sectors_read++;
    file->current_sector = next;
    file->current_offset_in_sector = 0;
    *bytes_read = bytes;
    return 0;
}

static uint32_t calc_current_pointer (file_descriptor * file)
{
    return file->sectors_read * file->FAT_info->global_info.sector_size + 
        file->current_offset_in_sector;
};

static bool too_many_for_read
(
    file_descriptor * file,
    uint32_t bytes
)
{
    uint32_t current_pointer = calc_current_pointer(file);
    return ((current_pointer + bytes > file->size) ||
        (current_pointer + bytes < current_pointer));
}

uint32_t f_read 
(
    file_descriptor * file, 
    void * buffer, 
    uint32_t bytes, 
    uint32_t * bytes_read
)
{
    *bytes_read = 0;
    [[unlikely]] if (file->current_sector == 0)
        return err::eof_file;
    
    [[unlikely]] if (bytes == 0)
        return 0;

    if (bytes > file->FAT_info->global_info.sector_size)
        bytes = file->FAT_info->global_info.sector_size;
    [[unlikely]] if (too_many_for_read(file, bytes))
    {
        bytes = file->size - calc_current_pointer(file);
        if (bytes == 0)
            return err::eof_file;
    }
    
    if (file->current_offset_in_sector != 0)
    {
        if (file->FAT_info->global_info.sector_size - file->current_offset_in_sector < bytes)
            bytes = file->FAT_info->global_info.sector_size - file->current_offset_in_sector;
        memcpy(buffer, file->buffer + file->current_offset_in_sector, bytes);
        if (file->current_offset_in_sector + bytes < file->FAT_info->global_info.sector_size)
        {
            file->current_offset_in_sector += bytes;
            *bytes_read = bytes;
            return 0;
        }
        else
        {
            return f_read_next_sector(file, bytes, bytes_read);
        }
    }
    else
    {
        if (bytes >= file->FAT_info->global_info.sector_size)
        {
            uint32_t ret;
            [[unlikely]] if ((ret = file->FAT_info->read_sector(file->current_sector, buffer)))
            {
                *bytes_read = 0;
                return ret; 
            }
            return f_read_next_sector(file, file->FAT_info->global_info.sector_size, bytes_read);
        }
        else
        {
            uint32_t ret;
            [[unlikely]] if ((ret = file->FAT_info->read_sector(file->current_sector, file->buffer)))
            {
                *bytes_read = 0;
                return ret; 
            }
            file->current_offset_in_sector = bytes;
            memcpy(buffer, file->buffer, bytes);
            *bytes_read = bytes;
            return 0;
        }
    }
}





uint32_t f_seek
(
    file_descriptor * file, 
    uint32_t bytes
)
{
    [[unlikely]] if (file->current_sector == 0)
        return 0;
    [[unlikely]] if (bytes > file->size)
        bytes = file->size;
    
    uint32_t offset_sector_in_cluster = file->current_sector & (file->FAT_info->global_info.cluster_size - 1);
    uint32_t current_sector = file->current_sector - offset_sector_in_cluster;
    uint32_t sectors_read = file->sectors_read - offset_sector_in_cluster;
    
    if (sectors_read * file->FAT_info->global_info.sector_size > bytes)
    {
        current_sector = file->start_sector;
        sectors_read = 0;
    }
    else
    {
        bytes -= sectors_read * file->FAT_info->global_info.sector_size;
    }

    while (bytes >= file->FAT_info->global_info.sector_size)
    {
        if (bytes >= file->FAT_info->global_info.sector_size * file->FAT_info->global_info.cluster_size)
        {
            uint32_t next, ret;
            [[unlikely]] if ((ret = file->FAT_info->next_sector(current_sector + file->FAT_info->global_info.cluster_size - 1, next)))
            {
                [[likely]] if (ret == err::eof_cluster)
                {
                    uint32_t s_rd = file->size % (file->FAT_info->global_info.sector_size * file->FAT_info->global_info.cluster_size);
                    sectors_read += s_rd / file->FAT_info->global_info.sector_size;
                    file->current_sector = 0;
                    file->current_offset_in_sector = s_rd % file->FAT_info->global_info.sector_size;
                    file->sectors_read = sectors_read;
                    return 0;
                }
                else
                {
                    return ret;
                }
            }
            sectors_read += file->FAT_info->global_info.cluster_size;
            current_sector = next;
            bytes -= file->FAT_info->global_info.sector_size * file->FAT_info->global_info.cluster_size;
        }
        else
        {
            current_sector += bytes / file->FAT_info->global_info.sector_size;
            sectors_read += bytes / file->FAT_info->global_info.sector_size;
            bytes %= file->FAT_info->global_info.sector_size;
            break;
        }
    }
    
    uint32_t ret;
    uint8_t need_reread = 
        ((file->current_sector != current_sector) ||
        (file->current_offset_in_sector == 0)) &&
        (bytes != 0);
    if (need_reread && (ret = file->FAT_info->read_sector(current_sector, file->buffer)))
        return ret; 
    file->current_sector = current_sector;
    file->current_offset_in_sector = bytes;
    file->sectors_read = sectors_read;
    return 0;
}

static uint32_t f_read_all_common
(
    file_descriptor * file, 
    void * buffer, 
    uint32_t bytes, 
    uint32_t * bytes_read
)
{
    uint32_t current_sector = file->current_sector;
    uint32_t current_offset_in_sector = file->current_offset_in_sector;
    uint32_t sectors_read = file->sectors_read; 
    char old_buff[max_sector_size];
    if (current_offset_in_sector != 0)
        memcpy(old_buff, file->buffer + current_offset_in_sector, file->FAT_info->global_info.sector_size - current_offset_in_sector);
    
    uint32_t ret, rb, rb_sum = 0;
    while (rb_sum != bytes)
    {
        uint32_t cnt_try = 0;
        while ((ret = f_read(file, (char *)buffer + rb_sum, bytes - rb_sum, &rb)) && (cnt_try < 3))
            cnt_try++;
        [[unlikely]] if (ret)
        {
            file->current_sector = current_sector;
            file->current_offset_in_sector = current_offset_in_sector;
            file->sectors_read = sectors_read; 
            if (current_offset_in_sector != 0)
                memcpy(file->buffer + current_offset_in_sector, old_buff, file->FAT_info->global_info.sector_size - current_offset_in_sector);
            *bytes_read = 0; //rb_sum;
            return ret;
        }
        rb_sum += rb;
    }
    *bytes_read = rb_sum;
    return 0;
}

uint32_t f_read_all_fixed
(
    file_descriptor * file, 
    void * buffer, 
    uint32_t bytes, 
    uint32_t * bytes_read
)
{
    *bytes_read = 0;
    [[unlikely]] if (file->current_sector == 0)
        return err::eof_file;
    
    [[unlikely]] if (too_many_for_read(file, bytes))
        return err::eof_file;
    
    return f_read_all_common(file, buffer, bytes, bytes_read);
}


uint32_t f_read_all
(
    file_descriptor * file, 
    void * buffer, 
    uint32_t bytes, 
    uint32_t * bytes_read
)
{
    *bytes_read = 0;
    [[unlikely]] if (file->current_sector == 0)
        return err::eof_file;
    
    [[unlikely]] if (too_many_for_read(file, bytes))
    {
        bytes = file->size - calc_current_pointer(file);
        if (bytes == 0)
            return err::eof_file;
    }
 
    return f_read_all_common(file, buffer, bytes, bytes_read);
}


