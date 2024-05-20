cmake_minimum_required(VERSION 3.27)
project(Kasp C)

set(CMAKE_C_STANDARD 11)
include_directories(/opt/homebrew/Cellar/argp-standalone/1.3/include)

add_executable(zip zip/main.c
        arg_parsing.c
        file_manipulations.c
        zip/compress_zlib.c
        zip/compress.c)
target_link_directories(zip PUBLIC /opt/homebrew/Cellar/argp-standalone/1.3/lib)
target_link_libraries(zip argp z)

add_executable(unzip unzip/main.c
        arg_parsing.c
        file_manipulations.c
        unzip/decompress_zlib.c
        unzip/decompress.c)
target_link_directories(unzip PUBLIC /opt/homebrew/Cellar/argp-standalone/1.3/lib)
target_link_libraries(unzip argp z)
