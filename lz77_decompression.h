//
// Created by Амир Кашапов on 18.05.2024.
//

#ifndef KASP_LZ77DECOMPRESSION_H
#define KASP_LZ77DECOMPRESSION_H
#include <errno.h>
#include <stdlib.h>
#include "lz77.h"
errno_t lz77_decompress_by_fd(int dst_fd, int src_fd, size_t blocksize);
#endif //KASP_LZ77DECOMPRESSION_H
