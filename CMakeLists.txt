cmake_minimum_required(VERSION 3.27)
project(Kasp C)

set(CMAKE_C_STANDARD 11)
include_directories(/opt/homebrew/Cellar/argp-standalone/1.3/include)

add_executable(Kasp main.c
        arg_parsing.c
        lz77_decompression.c
        lz77_compression.c
        lz77.c
        file_manipulations.c)
target_link_directories(Kasp PUBLIC /opt/homebrew/Cellar/argp-standalone/1.3/lib)
target_link_libraries(Kasp argp)
