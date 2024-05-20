//
// Created by Амир Кашапов on 18.05.2024.
//

#ifndef KASP_LZ77COMPRESSION_H
#define KASP_LZ77COMPRESSION_H
#include <errno.h>
#include "lz77.h"
errno_t lz77_compress_byte_array(char* dst, char* src);
#endif //KASP_LZ77COMPRESSION_H
