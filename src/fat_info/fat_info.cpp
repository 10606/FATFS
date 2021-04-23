#include "../structures/structure.h"
#include "fat_info.h"

namespace err
{
    const uint32_t invalid_cluster = 1;
    const uint32_t eof_cluster = 2;
    const uint32_t eof_file = 3;
};

struct total_MBR
{
    start_MBR start_MBR_v;
    MBR MBR_v;
    char tail[max_sector_size - sizeof(start_MBR) - sizeof(MBR)];
};

uint32_t FAT_info_t::init ()
{
    static_assert(sizeof(start_MBR) == 0x24, "wrong sizeof start_MBR");
    static_assert(sizeof(MBR) == 0x34 - 0x24 + 12, "wrong sizeof MBR");

    total_MBR buffer;
    
    uint32_t ret;
    if ((ret = read_sector(start_partition_sector, &buffer)))
    {
        return ret;
    }
    
    uint32_t FAT_count = buffer.start_MBR_v.FAT_count;
    global_info.cluster_size = buffer.start_MBR_v.sectors_in_cluster;
    global_info.FAT1 = global_info.cluster_size + start_partition_sector;
    
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

