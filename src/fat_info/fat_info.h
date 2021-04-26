#ifndef STRUCTURES__H
#define STRUCTURES__H

#include <stdint.h>

#ifndef max_sector_size
#define max_sector_size 512
#endif

namespace err
{
    extern const uint32_t invalid_cluster;
    extern const uint32_t eof_cluster;
    extern const uint32_t eof_file;
};

struct global_info_t
{
    uint32_t FAT1; //start sector
    uint32_t start_clusters; //start sector of 2'nd cluster
                            //TODO maybe start sector of 0'nd cluster
    uint32_t root_dir; //first sector of it
    //kto blyat numeruet clusters from 2
    
    uint32_t sector_size; //in bytes
    uint32_t cluster_size; //in sectors (maybe need 2^ )
};

struct FAT1_t
{
    uint32_t sector[max_sector_size / sizeof(uint32_t)];
    uint32_t current_sector;
};

struct FAT_info_t
{
    FAT_info_t 
    (
        uint32_t _sector_size,
        uint32_t _start_partition_sector, 
        uint32_t (* _read_sector) (uint32_t sector_number, void * buffer)
    ) :
        read_sector(_read_sector),
        start_partition_sector(_start_partition_sector)
    {
        global_info.sector_size = _sector_size;
        FAT1.current_sector = 0;
    }

    FAT_info_t (FAT_info_t const &) = delete;
    FAT_info_t (FAT_info_t &&) = delete;
    FAT_info_t & operator = (FAT_info_t const &) = delete;
    FAT_info_t & operator = (FAT_info_t &&) = delete;

    uint32_t init ();
    uint32_t next_sector (uint32_t sector, uint32_t & answer);
    uint32_t load_FAT (uint32_t sector, uint32_t & answer);

    friend struct file_descriptor;

    uint32_t (* read_sector) (uint32_t sector_number, void * buffer);
    uint32_t start_partition_sector;
    global_info_t global_info;
    FAT1_t FAT1;
};

#endif
