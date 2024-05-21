#ifndef KASP_COMPRESS_ZLIB_H
#define KASP_COMPRESS_ZLIB_H
#include <zlib.h>
#include "file_manipulations.h"

int compress_zlib(struct file_map* output, struct file_map* input);
#endif //KASP_COMPRESS_ZLIB_H
