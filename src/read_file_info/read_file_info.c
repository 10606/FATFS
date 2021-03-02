#include "read_file_info.h"

#include <string.h>
#include "../read/read.h"
#include "../structures/structure.h"
#include "../structures/structure_.h"

const uint32_t max_size = 0xfffffff0;
const uint32_t end_of_dir = 21;
const uint32_t not_short_file_name = 22;

uint32_t read_file_info 
(
    FAT_info_t * FAT_info, 
    short_file_name * sfn, 
    file_descriptor * fd, 
    char * file_name
)
{
    if ((sfn->attr & 0x0f) == 0x0f)
        return not_short_file_name;

    fd->FAT_info = FAT_info;
    fd->size = sfn->file_size;
    uint32_t first_cluster = (sfn->first_cluster_h << 16) + sfn->first_cluster_l; 

    fd->start_sector = 
        FAT_info->global_info.start_clusters + 
        (first_cluster - 2) * FAT_info->global_info.cluster_size;

    fd->current_sector = fd->start_sector;
    fd->current_offset_in_sector = 0;
    fd->sectors_read = 0;
    fd->is_dir = sfn->attr & 0x10;
    memcpy(file_name, sfn->name, 11);
    file_name[11] = 0;

    if (fd->is_dir)
        fd->size = max_size;
    
    return 0;
}

uint32_t read_dir 
(
    file_descriptor * fd, 
    file_descriptor * dst, 
    char * file_name
)
{
    uint32_t bread;
    short_file_name sfn;
    uint32_t cur_read = 0;
    while (1)
    {   
        uint32_t ret;
        if ((ret = f_read(fd, &sfn, sizeof(short_file_name) - cur_read, &bread)))
            return ret;
        cur_read += bread;
        if (cur_read != sizeof(short_file_name))
            continue;
        cur_read = 0;

        uint8_t b0 = sfn.name[0];
        if (b0 == 0xe5)
            continue;
        if (b0 == 0x00)
            return end_of_dir;
        
        if (!read_file_info(fd->FAT_info, &sfn, dst, file_name))
            return 0;
    }
}

