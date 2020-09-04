cmake_minimum_required(VERSION 3.0)

project(polar_codes VERSION 1.0 LANGUAGES C CXX)

set(my_release_flags
    "-O3 -mtune=native -march=native -g -Wall -ftemplate-depth=10000 -std=c++17"
)

set(release_flags
    "-O2 -Wall -ftemplate-depth=10000 -std=c++17"
)

set(compile_flags "-fsanitize=address -fsanitize=leak -fsanitize=undefined \
-fdiagnostics-color=always -fdiagnostics-generate-patch \
-g -Wall -ftemplate-depth=10000 -std=c++17"
)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${compile_flags}")

message("CMAKE_CXX_FLAGS" ${CMAKE_CXX_FLAGS})

add_library(FAT_fs STATIC
    src/file_descriptor/file_descriptor.c
    src/init/init.c
    src/load_FAT/load_FAT.c
    src/open/open.c
    src/read/read.c
    src/read_file_info/read_file_info.c
)


add_executable(read_file.cpp.elf
    test/main.cpp 
)
target_link_libraries(read_file.cpp.elf
    FAT_fs
)


add_executable(tree.cpp.elf
    test/tree.cpp 
)
target_link_libraries(tree.cpp.elf
    FAT_fs
)

#add_executable(time_test.cpp.elf
#    time_test.cpp 
#    variants/lin_depend.cpp
#    frozen_symbols/polar_subcodes.cpp
#)

