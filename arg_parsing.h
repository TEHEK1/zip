//
// Created by Амир Кашапов on 18.05.2024.
//

#ifndef KASP_ARG_PARSING_H
#define KASP_ARG_PARSING_H
#include <errno.h>
#include <stdlib.h>
struct args{
    size_t blocksize;
    char verbose;
    char* input_filename;
    char* output_filename;
};
errno_t parse_args(int argc, char **argv, struct args *args);
#endif //KASP_ARG_PARSING_H
