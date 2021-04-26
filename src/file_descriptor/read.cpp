#include "file_descriptor.h"

#include <stdint.h>
#include <string.h>
#include "../fat_info/fat_info.h"


uint32_t file_descriptor::read_next_sector (uint32_t bytes, uint32_t * bytes_read)
{
    uint32_t next, ret;
    if ((ret = FAT_info->next_sector(current_sector, next))) [[unlikely]] 
    {
        if (ret == err::eof_cluster) [[likely]] 
        {
            current_sector = 0;
            current_offset_in_sector = size % FAT_info->global_info.sector_size;
            *bytes_read = bytes;
            return 0;
        }
        else
        {
            *bytes_read = 0;
            return ret;
        }
    }
    sectors_read++;
    current_sector = next;
    current_offset_in_sector = 0;
    *bytes_read = bytes;
    return 0;
}


bool file_descriptor::too_many_for_read (uint32_t bytes)
{
    uint32_t current_pointer = current_position();
    return ((current_pointer + bytes > size) ||
        (current_pointer + bytes < current_pointer));
}


uint32_t file_descriptor::read 
(
    void * buff, 
    uint32_t bytes, 
    uint32_t * bytes_read
)
{
    *bytes_read = 0;
    if (current_sector == 0) [[unlikely]] 
        return err::eof_file;
    
    if (bytes == 0) [[unlikely]] 
        return 0;

    if (bytes > FAT_info->global_info.sector_size)
        bytes = FAT_info->global_info.sector_size;
    if (too_many_for_read(bytes)) [[unlikely]] 
    {
        bytes = size - current_position();
        if (bytes == 0)
            return err::eof_file;
    }
    
    if (current_offset_in_sector != 0)
    {
        if (FAT_info->global_info.sector_size - current_offset_in_sector < bytes)
            bytes = FAT_info->global_info.sector_size - current_offset_in_sector;
        memcpy(buff, buffer + current_offset_in_sector, bytes);
        if (current_offset_in_sector + bytes < FAT_info->global_info.sector_size)
        {
            current_offset_in_sector += bytes;
            *bytes_read = bytes;
            return 0;
        }
        else
        {
            return read_next_sector(bytes, bytes_read);
        }
    }
    else
    {
        uint32_t ret;
        if (bytes >= FAT_info->global_info.sector_size)
        {
            if ((ret = FAT_info->read_sector(current_sector, buff))) [[unlikely]] 
            {
                *bytes_read = 0;
                return ret; 
            }
            return read_next_sector(FAT_info->global_info.sector_size, bytes_read);
        }
        else
        {
            if ((ret = FAT_info->read_sector(current_sector, buffer))) [[unlikely]] 
            {
                *bytes_read = 0;
                return ret; 
            }
            current_offset_in_sector = bytes;
            memcpy(buff, buffer, bytes);
            *bytes_read = bytes;
            return 0;
        }
    }
}


uint32_t file_descriptor::seek (uint32_t bytes)
{
    if (current_sector == 0) [[unlikely]] 
        return 0;
    if (bytes > size) [[unlikely]] 
        bytes = size;
    
    uint32_t offset_sector_in_cluster = current_sector & (FAT_info->global_info.cluster_size - 1);
    uint32_t new_current_sector = current_sector - offset_sector_in_cluster;
    uint32_t new_sectors_read = sectors_read - offset_sector_in_cluster;
    
    if (new_sectors_read * FAT_info->global_info.sector_size > bytes)
    {
        new_current_sector = start_sector;
        new_sectors_read = 0;
    }
    else
    {
        bytes -= new_sectors_read * FAT_info->global_info.sector_size;
    }

    while (bytes >= FAT_info->global_info.sector_size)
    {
        if (bytes >= FAT_info->global_info.sector_size * FAT_info->global_info.cluster_size)
        {
            uint32_t next, ret;
            if ((ret = FAT_info->next_sector(new_current_sector + FAT_info->global_info.cluster_size - 1, next))) [[unlikely]] 
            {
                if (ret == err::eof_cluster) [[likely]] 
                {
                    uint32_t s_rd = size % (FAT_info->global_info.sector_size * FAT_info->global_info.cluster_size);
                    new_sectors_read += s_rd / FAT_info->global_info.sector_size;
                    current_sector = 0;
                    current_offset_in_sector = s_rd % FAT_info->global_info.sector_size;
                    sectors_read = new_sectors_read;
                    return 0;
                }
                else
                {
                    return ret;
                }
            }
            new_sectors_read += FAT_info->global_info.cluster_size;
            new_current_sector = next;
            bytes -= FAT_info->global_info.sector_size * FAT_info->global_info.cluster_size;
        }
        else
        {
            new_current_sector += bytes / FAT_info->global_info.sector_size;
            new_sectors_read += bytes / FAT_info->global_info.sector_size;
            bytes %= FAT_info->global_info.sector_size;
            break;
        }
    }
    
    uint32_t ret;
    bool need_reread = 
        ((current_sector != new_current_sector) ||
        (current_offset_in_sector == 0)) &&
        (bytes != 0);
    if (need_reread && (ret = FAT_info->read_sector(new_current_sector, buffer)))
        return ret; 
    current_sector = new_current_sector;
    current_offset_in_sector = bytes;
    sectors_read = new_sectors_read;
    return 0;
}


uint32_t file_descriptor::read_all_common
(
    void * buff, 
    uint32_t bytes, 
    uint32_t * bytes_read
)
{
    uint32_t old_current_sector = current_sector;
    uint32_t old_current_offset_in_sector = current_offset_in_sector;
    uint32_t old_sectors_read = sectors_read; 
    char old_buff[max_sector_size];
    if (old_current_offset_in_sector != 0)
        memcpy(old_buff, buffer + old_current_offset_in_sector, FAT_info->global_info.sector_size - old_current_offset_in_sector);
    
    uint32_t ret, rb, rb_sum = 0;
    while (rb_sum != bytes)
    {
        uint32_t cnt_try = 0;
        while ((ret = read((char *)buff + rb_sum, bytes - rb_sum, &rb)) && (cnt_try < 3))
            cnt_try++;
        if (ret) [[unlikely]] 
        {
            current_sector = old_current_sector;
            current_offset_in_sector = old_current_offset_in_sector;
            sectors_read = old_sectors_read; 
            if (old_current_offset_in_sector != 0)
                memcpy(buffer + old_current_offset_in_sector, old_buff, FAT_info->global_info.sector_size - old_current_offset_in_sector);
            *bytes_read = 0; //rb_sum;
            return ret;
        }
        rb_sum += rb;
    }
    *bytes_read = rb_sum;
    return 0;
}


uint32_t file_descriptor::read_all_fixed
(
    void * buff, 
    uint32_t bytes
)
{
    if (current_sector == 0) [[unlikely]] 
        return err::eof_file;
    
    if (too_many_for_read(bytes)) [[unlikely]] 
        return err::eof_file;
    
    uint32_t br;
    return read_all_common(buff, bytes, &br);
}


uint32_t file_descriptor::read_all
(
    void * buff, 
    uint32_t bytes, 
    uint32_t * bytes_read
)
{
    *bytes_read = 0;
    if (current_sector == 0) [[unlikely]] 
        return err::eof_file;
    
    if (too_many_for_read(bytes)) [[unlikely]] 
    {
        bytes = size - current_position();
        if (bytes == 0)
            return err::eof_file;
    }
 
    return read_all_common(buff, bytes, bytes_read);
}


