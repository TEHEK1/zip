//
// Created by Амир Кашапов on 20.05.2024.
//

#ifndef KASP_COMPRESS_H
#define KASP_COMPRESS_H
#include <errno.h>
#include <unistd.h>
errno_t fd_compress(int output_fd, int input_fd, size_t blocksize);
#endif //KASP_COMPRESS_H
