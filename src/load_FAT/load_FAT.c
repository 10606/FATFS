#include <stdint.h>
#include "load_FAT.h"
#include "../structures/structure_.h"
#include "../read/read_sector.h"

const uint32_t invalid_cluster = 1;
const uint32_t eof_cluster = 2;
const uint32_t eof_file = 3;

uint32_t load_FAT (uint32_t sector, uint32_t * answer)
{
    uint32_t cluster = (sector - global_info.start_clusters) / 
                        global_info.cluster_size + 2;

    uint32_t FAT_sector = global_info.FAT1 + 
                        cluster * sizeof(uint32_t) / global_info.sector_size;

    if (FAT_sector != FAT1.current_sector) 
    {
        uint32_t ret;
        if ((ret = read_sector(FAT_sector, FAT1.sector)))
        {
            *answer = 0xffffff0; //invalid cluster
            return ret;
        }
        FAT1.current_sector = FAT_sector;
    }
    *answer = FAT1.sector[cluster % (global_info.sector_size / sizeof(uint32_t))];
    return 0;
}

static inline char valid_cluster (uint32_t cluster)
{
    return  ((cluster & 0xfffffff) <= 0xfffffef) &&
            ((cluster & 0xfffffff) >= 0x0000002);
}

static inline char check_eof (uint32_t cluster)
{
    return  ((cluster & 0xfffffff) <= 0xfffffff) &&
            ((cluster & 0xfffffff) >= 0xffffff8);
}

uint32_t next_sector (uint32_t sector, uint32_t * answer)
{
    if ((sector - global_info.start_clusters + 1) % global_info.cluster_size == 0)
    {
        uint32_t cluster, ret;
        if ((ret = load_FAT(sector, &cluster)))
        {
            return ret;
        }
        if (valid_cluster(cluster))
        {
            *answer = (cluster - 2) * global_info.cluster_size +
                    global_info.start_clusters;
            return 0;
        }
        else
        {   
            *answer = 0;
            if (check_eof(cluster))
            {
                return eof_cluster;
            }
            return invalid_cluster; 
        }
    }
    else
    {
        *answer = sector + 1;
        return 0;
    }
}
