#ifndef KASP_COMPRESS_H
#define KASP_COMPRESS_H
#include <errno.h>
#include <stddef.h>

int fd_compress(int output_fd, int input_fd, size_t blocksize);
#endif //KASP_COMPRESS_H
