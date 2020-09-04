#include "../structures/structure.h"
#include "../structures/structure_.h"
#include "../read/read_sector.h"

#include "init.h"

global_info_t global_info;
FAT1_t FAT1 = {{0}, 0};


uint32_t init_fatfs ()
{
    //static_assert(sizeof(start_MBR) == 0x24);
    //static_assert(sizeof(MBR) == 0x34 - 0x24 + 12);

    FAT1.current_sector = 0;
    
    uint32_t buffer[max_sector_size / sizeof(uint32_t)];
    
    uint32_t ret;
    if ((ret = read_sector(start_partition_sector, buffer)))
    {
        return ret;
    }
    
    start_MBR * start_MBR_p = (start_MBR *)buffer;
    MBR * MBR_p = (MBR *) (buffer + sizeof(start_MBR) / sizeof(uint32_t));
    
    uint32_t FAT_count = start_MBR_p->FAT_count;
    //std::cout << "FAT_count " << FAT_count << "\n";
    global_info.cluster_size = start_MBR_p->sectors_in_cluster;
    global_info.FAT1 = global_info.cluster_size + start_partition_sector;
    global_info.sector_size = 
        *(uint16_t *) (start_MBR_p->bytes_in_sector);
    //*reinterpret_cast <uint8_t *> (&global_info.sector_size + 0) = 
    //    start_MBR_p->bytes_in_sector[0];
    //*reinterpret_cast <uint8_t *> (&global_info.sector_size + 1) = 
    //    start_MBR_p->bytes_in_sector[1];

    global_info.start_clusters = 
        global_info.FAT1 + 
        FAT_count * MBR_p->sectors_in_FAT;

    global_info.root_dir = global_info.start_clusters + 
        (MBR_p->root_cluster - 2) * global_info.cluster_size;
    return 0;
};


