//
// Created by Амир Кашапов on 18.05.2024.
//

#ifndef KASP_ARGPARSING_H
#define KASP_ARGPARSING_H
#include "stdlib.h"
struct args{
    size_t blocksize;
    char verbose;
    char* input_filename;
    char* output_filename;
};
#endif //KASP_ARGPARSING_H
