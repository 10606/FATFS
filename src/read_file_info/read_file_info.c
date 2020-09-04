#include "read_file_info.h"

#include <string.h>
#include "../read/read.h"
#include "../structures/structure.h"
#include "../structures/structure_.h"

const uint32_t max_size = 0xfffffff0;
const uint32_t end_of_dir = 21;
const uint32_t not_short_file_name = 22;

uint32_t read_file_info (uint8_t * file_info, file_descriptor * fd, char * file_name)
{
    //long_file_name * lfn = 
    //    reinterpret_cast <long_file_name *> (file_info);
    short_file_name * sfn = 
        (short_file_name *) (file_info);

    if ((sfn->attr & 0x0f) == 0x0f)
    {
        return not_short_file_name;
    }

    fd->size = sfn->file_size;
    uint32_t first_cluster = (sfn->first_cluster_h << 16) + sfn->first_cluster_l; 

    fd->start_sector = 
        global_info.start_clusters + 
        (first_cluster - 2) * global_info.cluster_size;

    fd->current_sector = fd->start_sector;
    fd->current_offset_in_sector = 0;
    fd->sectors_read = 0;
    fd->is_dir = sfn->attr & 0x10;
    memcpy(file_name, sfn->name, 11);
    file_name[11] = 0;

    if (fd->is_dir)
    {
        fd->size = max_size;
    }
    
    return 0;
}

uint32_t read_dir (file_descriptor * fd, file_descriptor * dst,  char * file_name)
{
    uint8_t buffer[max_sector_size];
    uint32_t bread;

    while (1)
    {   
        uint32_t ret;
        if ((ret = f_read(fd, buffer, sizeof(short_file_name)/*max_sector_size*/, &bread)))
        {
            return ret;
        }

        for (uint32_t record = 0; record != bread / sizeof(short_file_name); ++record)
        {
            uint8_t b0 = buffer[record * sizeof(short_file_name)];
            if (b0 == 0xe5)
            {
                continue;
            }
            if (b0 == 0x00)
            {
                return end_of_dir;
            }
            
            if (!read_file_info(buffer + record * sizeof(short_file_name), dst, file_name))
            {
                return 0;
            }
        }
    }
}

