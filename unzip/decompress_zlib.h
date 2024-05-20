//
// Created by Амир Кашапов on 20.05.2024.
//

#ifndef KASP_DECOMPRESS_ZLIB_H
#define KASP_DECOMPRESS_ZLIB_H
#include <zlib.h>
#include "../file_manipulations.h"
errno_t decompress_zlib(struct file_map* output, struct file_map* input);
#endif //KASP_DECOMPRESS_ZLIB_H
