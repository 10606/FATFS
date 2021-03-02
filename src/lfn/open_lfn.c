#include "open_lfn.h"

#include "read_lfn_info.h"
#include "../read_file_info/read_file_info.h"
#include <string.h>

uint32_t const not_found = 41;
uint32_t const path_consist_not_dir = 42;

char str_n_cmp (char const * a, char const * b, uint32_t size)
{
    for (size_t i = 0; i != size; ++i)
    {
        if (a[i] != b[i])
            return 1;
    }
    return 0;
}

uint32_t get_len_2 (char const * value)
{
    uint32_t answer = 0;
    for (; value[answer] != 0 || value[answer + 1] != 0; answer += 2);
    return answer;
}

uint32_t open_lfn
(
    FAT_info_t * FAT_info, 
    file_descriptor * file,
    char const * const * long_path, //[path_len][len_i]
    char (* path)[12], //[path_len][11]
    uint32_t path_len
)
{
    file_descriptor _dir =
    {
        max_size, 
        FAT_info->global_info.root_dir, 
        FAT_info->global_info.root_dir,
        0,
        0,
        FAT_info,
        1,
        {0}
    };
    
    if (path_len == 0)
    {
        *file = _dir;
        return 0;
    }
    
    file_descriptor _next;

    file_descriptor * dir = &_dir;
    file_descriptor * next = &_next;
    char long_name[(256 + 13) * 2];
    
    for (uint32_t i = 0; i != path_len; ++i)
    {
        while (1)
        {
            uint32_t ret;
            if ((ret = read_dir_lfn(dir, next, path[i], long_name)))
                return ret;

            uint32_t long_len = get_len_2(long_name);
            uint32_t path_i_len = get_len_2(long_path[i]);
            
            if ((long_len == path_i_len) &&
                (str_n_cmp(long_name, long_path[i], long_len) == 0))
            {
                if (i + 1 == path_len)
                {
                    *file = *next;
                    return 0;
                }
                else
                {
                    if (!next->is_dir)
                        return path_consist_not_dir;

                    file_descriptor * tmp = dir;
                    dir = next;
                    next = tmp;
                    break;
                }
            }
        }
    }
    
    return not_found;
}


