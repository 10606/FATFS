#ifndef STRUCTURES__H
#define STRUCTURES__H

#include <stdint.h>

#ifndef max_sector_size
#define max_sector_size 512
#endif

typedef struct global_info_t
{
    uint32_t FAT1; //start sector
    uint32_t start_clusters; //start sector of 2'nd cluster
                            //TODO maybe start sector of 0'nd cluster
    uint32_t root_dir; //first sector of it
    //kto blyat numeruet clusters from 2
    
    uint32_t sector_size; //in bytes
    uint32_t cluster_size; //in sectors (maybe need 2^ )
} global_info_t;

typedef struct FAT1_t
{
    uint32_t sector[max_sector_size / sizeof(uint32_t)];
    uint32_t current_sector;
} FAT1_t;

typedef struct 
{
    global_info_t global_info;
    FAT1_t FAT1;
    uint32_t start_partition_sector;
    uint32_t (* read_sector) (uint32_t sector_number, void * buffer);
} FAT_info_t;

typedef struct file_descriptor
{
    uint32_t size; //in bytes
    uint32_t start_sector;
    
    uint32_t current_sector; //not readed
    uint32_t current_offset_in_sector; //not readed
    uint32_t sectors_read; //count of readed sectors to compare with size 
    
    FAT_info_t * FAT_info;
    
    char is_dir;
    char buffer[max_sector_size];
} file_descriptor;

#endif
