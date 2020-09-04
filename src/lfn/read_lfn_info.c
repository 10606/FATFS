#include "read_lfn_info.h"
#include "../read_file_info/read_file_info.h"
#include "../read/read.h"
#include <string.h>

const uint32_t not_long_file_name = 31;

const uint32_t lfnp_len = 5 + 6 + 2;
uint32_t read_lfn_info (uint8_t * file_info, char * long_name)
{
    long_file_name * lfn = 
        (long_file_name *) (file_info);

    if (lfn->attr != 0x0f)
    {
        return not_long_file_name;
    }
    
    uint32_t offset = lfn->fragment_index - ((lfn->fragment_index > 0x40)? 0x40 : 0);
    offset = (offset - 1) * lfnp_len * 2;
    
    memcpy(long_name + offset,              lfn->name_0_4, 5*2);
    memcpy(long_name + offset + 5*2,        lfn->name_5_10, 6*2);
    memcpy(long_name + offset + 5*2 + 6*2,  lfn->name_11_12, 2*2);
    
    if (lfn->fragment_index > 0x40)
    {
        long_name[offset + lfnp_len * 2    ] = 0;
        long_name[offset + lfnp_len * 2 + 1] = 0;
    }
    
    return 0;
}

uint32_t read_dir_lfn (file_descriptor * fd, file_descriptor * dst,  char * file_name, char * long_name)
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
            
            read_lfn_info(buffer + record * sizeof(short_file_name), long_name);
            if (!read_file_info(buffer + record * sizeof(short_file_name), dst, file_name))
            {
                return 0;
            }
        }
    }
}

