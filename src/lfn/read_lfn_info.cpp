#include "read_lfn_info.h"
#include "../read_file_info/read_file_info.h"
#include "../file_descriptor/file_descriptor.h"
#include <string.h>

namespace err
{
    const uint32_t not_long_file_name = 31;
}

uint32_t read_lfn_info (long_file_name * lfn, char * long_name)
{
    static const uint32_t lfnp_len = 5 + 6 + 2;
    if (lfn->attr != 0x0f)
        return err::not_long_file_name;
    
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

uint32_t read_dir_lfn 
(
    file_descriptor * fd, 
    file_descriptor * dst,  
    char * file_name, 
    char * long_name
)
{
    uint32_t bread;
    short_file_name sfn;
    uint32_t cur_read = 0;
    while (1)
    {   
        uint32_t ret;
        if ((ret = fd->read(&sfn, sizeof(short_file_name) - cur_read, &bread)))
            return ret;
        cur_read += bread;
        if (cur_read != sizeof(short_file_name))
            continue;
        cur_read = 0;

        uint8_t b0 = sfn.name[0];
        if (b0 == 0xe5)
            continue;
        if (b0 == 0x00)
            return err::end_of_dir;
        
        read_lfn_info((long_file_name *)&sfn, long_name);
        if (!read_file_info(fd->FAT_info, &sfn, dst, file_name))
            return 0;
    }
}

