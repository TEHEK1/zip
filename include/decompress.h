#ifndef KASP_DECOMPRESS_H
#define KASP_DECOMPRESS_H
#include <errno.h>
#include <stddef.h>

int fd_decompress(int output_fd, int input_fd, size_t blocksize);
#endif //KASP_DECOMPRESS_H
