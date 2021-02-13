#include <stdint.h>
#include <string.h>
#include "read.h"
#include "read_sector.h"
#include "../load_FAT/load_FAT.h"

inline uint32_t min (uint32_t a, uint32_t b)
{   
    if (a < b)
    {
        return a;
    }
    else
    {
        return b;
    }
};

uint32_t f_read 
(
    file_descriptor * file, 
    void * buffer, 
    uint32_t bytes, 
    uint32_t * bytes_read
)
{
    *bytes_read = 0;
    if (file->current_sector == 0)
        return eof_file;
    
    if (bytes == 0)
        return 0;

    uint32_t current_pointer = 
        (file->sectors_read) * global_info.sector_size + 
        file->current_offset_in_sector;
    if (bytes > global_info.sector_size)
    {
        bytes = global_info.sector_size;
    }
    if ((current_pointer + bytes > file->size) ||
        (current_pointer + bytes < current_pointer))
    {
        bytes = file->size - current_pointer;
        if (bytes == 0)
        {
            *bytes_read = 0;
            return eof_file;
        }
    }
    
    if (file->current_offset_in_sector != 0)
    {
        if (global_info.sector_size - file->current_offset_in_sector < bytes)
        {
            bytes = global_info.sector_size - file->current_offset_in_sector;
        }
        memcpy(buffer, file->buffer + file->current_offset_in_sector, bytes);
        if (file->current_offset_in_sector + bytes < global_info.sector_size)
        {
            file->current_offset_in_sector += bytes;
            *bytes_read = bytes;
            return 0;
        }
        else
        {
            uint32_t next, ret;
            if ((ret = next_sector(file->current_sector, &next)))
            {
                if (ret == eof_cluster)
                {
                    file->current_sector = 0;
                    file->current_offset_in_sector = file->size % global_info.sector_size;
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
    }
    else
    {
        if (bytes >= global_info.sector_size)
        {
            uint32_t ret;
            if ((ret = read_sector(file->current_sector, buffer)))
            {
                *bytes_read = 0;
                return ret; 
            }
            uint32_t next;
            if ((ret = next_sector(file->current_sector, &next)))
            {
                if (ret == eof_cluster)
                {
                    file->current_sector = 0;
                    file->current_offset_in_sector = file->size % global_info.sector_size;
                    *bytes_read = global_info.sector_size;
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
            *bytes_read = global_info.sector_size;
            return 0;
        }
        else
        {
            uint32_t ret;
            if ((ret = read_sector(file->current_sector, file->buffer)))
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
    if (bytes > file->size)
        bytes = file->size;
    
    uint32_t offset_sector_in_cluster = file->current_sector & (global_info.cluster_size - 1);
    uint32_t current_sector = file->current_sector - offset_sector_in_cluster;
    uint32_t sectors_read = file->sectors_read - offset_sector_in_cluster;
    
    if (sectors_read * global_info.sector_size > bytes)
    {
        current_sector = file->start_sector;
        sectors_read = 0;
    }
    else
    {
        bytes -= sectors_read * global_info.sector_size;
    }

    while (bytes >= global_info.sector_size)
    {
        if (bytes >= global_info.sector_size * global_info.cluster_size)
        {
            uint32_t next, ret;
            if ((ret = next_sector(current_sector + global_info.cluster_size - 1, &next)))
            {
                if (ret == eof_cluster)
                {
                    uint32_t s_rd = file->size % (global_info.sector_size * global_info.cluster_size);
                    sectors_read += s_rd / global_info.sector_size;
                    file->current_sector = 0;
                    file->current_offset_in_sector = s_rd % global_info.sector_size;
                    file->sectors_read = sectors_read;
                    return 0;
                }
                else
                {
                    return ret;
                }
            }
            sectors_read += global_info.cluster_size;
            current_sector = next;
            bytes -= global_info.sector_size * global_info.cluster_size;
        }
        else
        {
            current_sector += bytes / global_info.sector_size;
            sectors_read += bytes / global_info.sector_size;
            bytes %= global_info.sector_size;
            break;
        }
    }
    
    uint32_t ret;
    uint8_t need_reread = 
        ((file->current_sector != current_sector) ||
        (file->current_offset_in_sector == 0)) &&
        (bytes != 0);
    if (need_reread && (ret = read_sector(current_sector, file->buffer)))
        return ret; 
    file->current_sector = current_sector;
    file->current_offset_in_sector = bytes;
    file->sectors_read = sectors_read;
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

    if (file->current_sector == 0)
    {
        *bytes_read = 0;
        return eof_file;
    }
    
    uint32_t current_pointer = 
        (file->sectors_read) * global_info.sector_size + 
        file->current_offset_in_sector;
    if ((current_pointer + bytes > file->size) ||
        (current_pointer + bytes < current_pointer))
    {
        *bytes_read = 0;
        return eof_file;
    }
    
    uint32_t current_sector = file->current_sector;
    uint32_t current_offset_in_sector = file->current_offset_in_sector;
    uint32_t sectors_read = file->sectors_read; 
    char old_buff[max_sector_size];
    if (current_offset_in_sector != 0)
    {
        memcpy(old_buff, file->buffer + current_offset_in_sector, global_info.sector_size - current_offset_in_sector);
    }
    
    uint32_t ret, rb, rb_sum = 0;
    while (rb_sum != bytes)
    {
        uint32_t cnt_try = 0;
        while ((ret = f_read(file, (char *)buffer + rb_sum, bytes - rb_sum, &rb)) && (cnt_try < 3))
            cnt_try++;
        if (ret)
        {
            file->current_sector = current_sector;
            file->current_offset_in_sector = current_offset_in_sector;
            file->sectors_read = sectors_read; 
            if (current_offset_in_sector != 0)
            {
                memcpy(file->buffer + current_offset_in_sector, old_buff, global_info.sector_size - current_offset_in_sector);
            }
            *bytes_read = 0; //rb_sum;
            return ret;
        }
        rb_sum += rb;
    }
    *bytes_read = rb_sum;
    return 0;
}


uint32_t f_read_all
(
    file_descriptor * file, 
    void * buffer, 
    uint32_t bytes, 
    uint32_t * bytes_read
)
{
    uint32_t current_pointer = 
        (file->sectors_read) * global_info.sector_size + 
        file->current_offset_in_sector;
    if ((current_pointer + bytes > file->size) ||
        (current_pointer + bytes < current_pointer))
    {
        bytes = file->size - current_pointer;
        if (bytes == 0)
        {
            *bytes_read = 0;
            return eof_file;
        }
    }
 
    return f_read_all_fixed(file, buffer, bytes, bytes_read);
}





