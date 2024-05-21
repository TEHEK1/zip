cmake_minimum_required(VERSION 3.27)
project(Kasp C CXX)

set(CMAKE_C_STANDARD 11)
#set(CMAKE_C_FLAGS "-fsanitize=address -fsanitize=undefined -fno-sanitize-recover=all -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fno-sanitize=null -fno-sanitize=alignment")
set(CMAKE_CXX_FLAGS "-std=c++17")
include_directories(/opt/homebrew/Cellar/argp-standalone/1.3/include)
include_directories(include)
include_directories(googletest/googletest/include)
include_directories(googletest/googletest/)
add_executable(zip zip/main.c
        zip/arg_parsing.c
        file_manipulations.c
        zip/compress_zlib.c
        zip/compress.c)
target_link_directories(zip PUBLIC /opt/homebrew/Cellar/argp-standalone/1.3/lib)
target_link_libraries(zip argp z)

add_executable(unzip unzip/main.c
        unzip/arg_parsing.c
        file_manipulations.c
        unzip/decompress_zlib.c
        unzip/decompress.c
        unzip/arg_parsing.c)

target_link_directories(unzip PUBLIC /opt/homebrew/Cellar/argp-standalone/1.3/lib)
target_link_libraries(unzip argp z)

add_executable(Test tests/test.cpp
        zip/arg_parsing.c
        file_manipulations.c
        zip/compress_zlib.c
        zip/compress.c
        unzip/decompress.c
        unzip/decompress_zlib.c
        googletest/googletest/src/gtest-all.cc)
target_link_directories(Test PUBLIC /opt/homebrew/Cellar/argp-standalone/1.3/lib)
target_link_libraries(Test argp z)
