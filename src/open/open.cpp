#include "open.h"

#include "../read_file_info/read_file_info.h"
#include <string.h>
#include <algorithm>

namespace err
{
    uint32_t const not_found = 11;
    uint32_t const path_consist_not_dir = 12;
}

uint32_t open 
(
    FAT_info_t * FAT_info, 
    file_descriptor * file,
    char const (* const path)[12], //[path_len][11]
    uint32_t path_len
)
{
    file_descriptor _dir(FAT_info);
    
    if (path_len == 0)
    {
        *file = _dir;
        return 0;
    }
    
    file_descriptor _next;

    file_descriptor * dir = &_dir;
    file_descriptor * next = &_next;
    char name[12];
    
    for (uint32_t i = 0; i != path_len; ++i)
    {
        while (1)
        {
            uint32_t ret;
            if ((ret = read_dir(dir, next, name)))
            {
                return ret;
            }
            if (!strncmp(name, path[i], 11))
            {
                if (i + 1 == path_len)
                {
                    *file = *next;
                    return 0;
                }
                else
                {
                    if (!next->is_dir)
                    {
                        return err::path_consist_not_dir;
                    }
                    std::swap(dir, next);
                    break;
                }
            }
        }
    }
    
    return err::not_found;
}

