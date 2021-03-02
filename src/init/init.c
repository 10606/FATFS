#include "../structures/structure.h"
#include "../structures/structure_.h"

#include "init.h"

typedef struct total_MBR
{
    start_MBR start_MBR_v;
    MBR MBR_v;
    char tail[max_sector_size - sizeof(start_MBR) - sizeof(MBR)];
} total_MBR;

uint32_t init_fatfs 
(
    FAT_info_t * FAT_info, 
    uint32_t _sector_size,
    uint32_t _start_partition_sector, 
    uint32_t (* _read_sector) (uint32_t sector_number, void * buffer)
)
{
    FAT_info->global_info.sector_size = _sector_size;
    FAT_info->start_partition_sector = _start_partition_sector;
    FAT_info->read_sector = _read_sector;
    //static_assert(sizeof(start_MBR) == 0x24);
    //static_assert(sizeof(MBR) == 0x34 - 0x24 + 12);

    FAT_info->FAT1.current_sector = 0;
    total_MBR buffer;
    
    uint32_t ret;
    if ((ret = FAT_info->read_sector(FAT_info->start_partition_sector, &buffer)))
    {
        return ret;
    }
    
    uint32_t FAT_count = buffer.start_MBR_v.FAT_count;
    FAT_info->global_info.cluster_size = buffer.start_MBR_v.sectors_in_cluster;
    FAT_info->global_info.FAT1 = FAT_info->global_info.cluster_size + FAT_info->start_partition_sector;
    
    FAT_info->global_info.sector_size = 
        ((uint16_t)buffer.start_MBR_v.bytes_in_sector[0] + 
        ((uint16_t)buffer.start_MBR_v.bytes_in_sector[1] << 8));

    FAT_info->global_info.start_clusters = 
        FAT_info->global_info.FAT1 + 
        FAT_count * buffer.MBR_v.sectors_in_FAT;

    FAT_info->global_info.root_dir = FAT_info->global_info.start_clusters + 
        (buffer.MBR_v.root_cluster - 2) * FAT_info->global_info.cluster_size;
    return 0;
};

