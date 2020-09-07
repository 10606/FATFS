#include "../structures/structure.h"
#include "../structures/structure_.h"
#include "../read/read_sector.h"

#include "init.h"

global_info_t global_info;
FAT1_t FAT1 = {{0}, 0};

typedef struct total_MBR
{
    start_MBR start_MBR_v;
    MBR MBR_v;
    char tail[max_sector_size - sizeof(start_MBR) - sizeof(MBR)];
} total_MBR;

uint32_t init_fatfs ()
{
    //static_assert(sizeof(start_MBR) == 0x24);
    //static_assert(sizeof(MBR) == 0x34 - 0x24 + 12);

    FAT1.current_sector = 0;
    total_MBR buffer;
    
    uint32_t ret;
    if ((ret = read_sector(start_partition_sector, &buffer)))
    {
        return ret;
    }
    
    uint32_t FAT_count = buffer.start_MBR_v.FAT_count;
    global_info.cluster_size = buffer.start_MBR_v.sectors_in_cluster;
    global_info.FAT1 = global_info.cluster_size + start_partition_sector;
    //global_info.sector_size = 
    //    *(uint16_t *) (buffer.start_MBR_v.bytes_in_sector);
    global_info.sector_size = 
        ((uint16_t)buffer.start_MBR_v.bytes_in_sector[0] + 
        ((uint16_t)buffer.start_MBR_v.bytes_in_sector[1] << 8));

    global_info.start_clusters = 
        global_info.FAT1 + 
        FAT_count * buffer.MBR_v.sectors_in_FAT;

    global_info.root_dir = global_info.start_clusters + 
        (buffer.MBR_v.root_cluster - 2) * global_info.cluster_size;
    return 0;
};

