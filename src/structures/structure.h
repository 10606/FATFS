#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdint.h>

typedef struct start_MBR
{
    uint8_t jmp[3];
    char id_vendor[8];
    uint8_t bytes_in_sector[2]; //pidoracy ne smogli v vyravnivanie
    uint8_t sectors_in_cluster;
    
    uint16_t reserved_sectors;
    uint8_t FAT_count;
    
    uint8_t z0[2]; //0x0000
    
    uint8_t sectors_in_partition_16[2]; //0x0000 -> *(0x20) 
    //pidoracy ne smogli v vyravnivanie
    
    uint8_t disk_type; //0xf8
    uint8_t z1[2]; //0x0000
    
    uint16_t sectors_in_road;
    uint16_t side_count;
    
    uint32_t hidden_sectors;
    uint32_t sectors_in_partition;
} start_MBR;

typedef struct MBR
{
    uint32_t sectors_in_FAT;
    
    uint16_t active_FAT_index;
    uint16_t version; //0x0000
    uint32_t root_cluster;
    uint16_t FSINFO_sector_index;
    
    uint16_t MRB_reserve_sector_index;
    
    uint8_t z0[12]; //zahuya?
} MBR;

typedef struct long_file_name
{
    uint8_t fragment_index;
    
    char name_0_4[10]; //0-4
    //pidoracy
    uint8_t attr;
    
    uint8_t flags;
    uint8_t check_sum_short_name;
    char name_5_10[12]; //5-10
    //pidoracy
    uint16_t first_cluster; //0x0000
    char name_11_12[4];
    //pidoracy
} long_file_name;

typedef struct short_file_name
{
    char name[11]; //8.3
    
    uint8_t attr;
    
    uint8_t z0; //win NT
    uint8_t ctime_10ms;
    uint8_t ctime_time;
    uint16_t ctime_date;
    //UNIX time i ne vyebyvaytec'
    
    uint16_t atime_date;
    
    uint16_t first_cluster_h;
    //pidoracy
    uint16_t otime_time;
    uint16_t otime_date;
    uint16_t first_cluster_l;
    //pidoracy
    uint32_t file_size;
} short_file_name;

typedef struct FSINFO
{
    uint32_t z0; //0x41615252
    uint8_t z1[480]; //0x00000..00000
    uint32_t z2; //0x61417272
    uint32_t free_clusters_count; //0xffffffff -> find yourself
    uint32_t find_free_clusters_from; //0xffffffff -> find yourself
    uint8_t z3[12]; //0x0000...000000
    uint32_t z4; //0xaa550000
} FSINFO;

#endif
