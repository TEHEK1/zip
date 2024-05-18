cmake_minimum_required(VERSION 3.27)
project(Kasp C)

set(CMAKE_C_STANDARD 11)
include_directories(/opt/homebrew/Cellar/argp-standalone/1.3/include)
add_executable(Kasp main.c
        argparsing.c)
link_libraries(Kasp argp)
