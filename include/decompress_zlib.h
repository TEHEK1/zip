#ifndef KASP_DECOMPRESS_ZLIB_H
#define KASP_DECOMPRESS_ZLIB_H
#include <zlib.h>
#include "file_manipulations.h"

int decompress_zlib(struct file_map* output, struct file_map* input);
#endif //KASP_DECOMPRESS_ZLIB_H
