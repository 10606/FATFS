#ifndef FILE_DESCRIPTOR_H
#define FILE_DESCRIPTOR_H

#include "../fat_info/fat_info.h"
#include <cstring>

struct file_descriptor
{
    constexpr file_descriptor () :
        size(0),
        start_sector(0),
        
        current_sector(0),
        current_offset_in_sector(0),
        sectors_read(0),
     
        FAT_info(nullptr),
        buffer{},
        is_dir(0)
    {}
    
    constexpr file_descriptor (FAT_info_t * _FAT_info) :
        size(max_size),
        start_sector(_FAT_info->global_info.root_dir),
        
        current_sector(_FAT_info->global_info.root_dir),
        current_offset_in_sector(0),
        sectors_read(0),
        
        FAT_info(_FAT_info),
        
        buffer{},
        is_dir(1)
    {}
    
    file_descriptor (file_descriptor const & other)
    {
        copy(other);
    }

    void copy (file_descriptor const & other)
    {
        FAT_info = other.FAT_info;
        size = other.size; //in bytes
        start_sector = other.start_sector;

        current_sector = other.current_sector; //not readed
        current_offset_in_sector = other.current_offset_in_sector; //not readed
        sectors_read = other.sectors_read; //count of readed sectors to compare with size

        is_dir = other.is_dir;
        memcpy(buffer, other.buffer, max_sector_size);
    }

    file_descriptor (file_descriptor const & other, int)
    {
        copy_seek_0(other);
    }

    constexpr void copy_seek_0 (file_descriptor const & other)
    {
        FAT_info = other.FAT_info;
        size = other.size; //in bytes
        start_sector = other.start_sector;

        current_sector = start_sector; //not readed
        current_offset_in_sector = 0; //not readed
        sectors_read = 0; //count of readed sectors to compare with size

        is_dir = other.is_dir;
    }

    constexpr bool operator == (file_descriptor const & rhs)
    {
        return (FAT_info == rhs.FAT_info) && (start_sector == rhs.start_sector);
    }

    constexpr uint32_t current_position ()
    {
        if (start_sector == 0)
            return 0;
        if (current_sector == 0)
            return size;
        return (sectors_read) * FAT_info->global_info.sector_size + 
            current_offset_in_sector;
    }

    constexpr void init_fake ()
    {
        size = 0;
        start_sector = 0;
        
        current_sector = 0;
        current_offset_in_sector = 0;
        sectors_read = 0;
     
        FAT_info = nullptr;
        is_dir = 0;
    }

    constexpr bool is_fake ()
    {
        return (start_sector == 0) && (current_sector == 0);
    }
    
    

    uint32_t size; //in bytes
    uint32_t start_sector;
    
    uint32_t current_sector; //not readed
    uint32_t current_offset_in_sector; //not readed
    uint32_t sectors_read; //count of readed sectors to compare with size 
    
    FAT_info_t * FAT_info;
    
    char buffer[max_sector_size];
    bool is_dir;
    
    
    static constexpr uint32_t max_size = 0xfffffff0;
};

#endif

