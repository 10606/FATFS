#ifndef FILE_DESCRIPTOR_H
#define FILE_DESCRIPTOR_H

#include "../fat_info/fat_info.h"
#include <utility>
#include <cstring>

typedef char filename_t[12];

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
    
    constexpr file_descriptor (file_descriptor const & other) = default;
    constexpr file_descriptor & operator = (file_descriptor const & other) = default;
    constexpr file_descriptor (file_descriptor && other) = default;
    constexpr file_descriptor & operator = (file_descriptor && other) = default;

    constexpr file_descriptor (file_descriptor const & other, int) : // copy & seek 0
        size(other.size),
        start_sector(other.start_sector),

        current_sector(start_sector),
        current_offset_in_sector(0),
        sectors_read(0),

        FAT_info(other.FAT_info),
        buffer{},
        is_dir(other.is_dir)
    {}

    constexpr void copy_seek_0 (file_descriptor const & other) noexcept
    {
        FAT_info = other.FAT_info;
        size = other.size; //in bytes
        start_sector = other.start_sector;

        current_sector = start_sector; //not readed
        current_offset_in_sector = 0; //not readed
        sectors_read = 0; //count of readed sectors to compare with size

        is_dir = other.is_dir;
    }

    constexpr bool operator == (file_descriptor const & rhs) const noexcept
    {
        return (FAT_info == rhs.FAT_info) && (start_sector == rhs.start_sector);
    }

    constexpr uint32_t current_position () const noexcept
    {
        if (start_sector == 0)
            return 0;
        if (current_sector == 0)
            return size;
        return (sectors_read) * FAT_info->global_info.sector_size + 
            current_offset_in_sector;
    }

    // constexpr only in c++20, but i get many warnings from CMSIS 
    //  with deprecated |= for volatile
    /*constexpr*/ void swap (file_descriptor & other) noexcept
    {
        std::swap(size, other.size);
        std::swap(start_sector, other.start_sector);
        
        std::swap(current_sector, other.current_sector);
        std::swap(current_offset_in_sector, other.current_offset_in_sector);
        std::swap(sectors_read, other.sectors_read);
     
        std::swap(FAT_info, other.FAT_info);
        std::swap(buffer, other.buffer);
        std::swap(is_dir, other.is_dir);
    }

    constexpr void init_fake () noexcept
    {
        size = 0;
        start_sector = 0;
        
        current_sector = 0;
        current_offset_in_sector = 0;
        sectors_read = 0;
     
        FAT_info = nullptr;
        is_dir = 0;
    }

    constexpr bool is_fake () const noexcept
    {
        return (start_sector == 0) && (current_sector == 0);
    }
    
    uint32_t read (void * buff, uint32_t bytes, uint32_t * bytes_read);
    uint32_t read_all (void * buff, uint32_t bytes, uint32_t * bytes_read);
    uint32_t read_all_fixed (void * buff, uint32_t bytes);
    uint32_t seek (uint32_t bytes);

    

    uint32_t size; //in bytes
    uint32_t start_sector;
    
    uint32_t current_sector; //not readed
    uint32_t current_offset_in_sector; //not readed
    uint32_t sectors_read; //count of readed sectors to compare with size 
    
    FAT_info_t * FAT_info;
    
    char buffer[max_sector_size];
    bool is_dir;
    
    
    static constexpr uint32_t max_size = 0xfffffff0;
    
private:
    
    uint32_t read_next_sector (uint32_t bytes, uint32_t * bytes_read);
    bool too_many_for_read (uint32_t bytes);
    uint32_t read_all_common (void * buff, uint32_t bytes, uint32_t * bytes_read);
};

#endif

