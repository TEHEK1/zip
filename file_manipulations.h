//
// Created by Амир Кашапов on 18.05.2024.
//

#ifndef KASP_FILE_MANIPULATIONS_H
#define KASP_FILE_MANIPULATIONS_H
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
struct file_map{
    char* mapped_mem;
    size_t seek;
    size_t block_start;
    size_t block_end;
    size_t length;
    size_t blocksize;
    int prot;
    int fd;
};
errno_t init_file_map(struct file_map* out, int fd, size_t blocksize, void* dst, int prot, off_t seek);
char get_byte_file_map(struct file_map* input);
char put_byte_file_map(struct file_map* input, char new_byte);

errno_t update_check_file_map(struct file_map* out);

errno_t deinit_file_map(struct file_map* out);
#endif //KASP_FILE_MANIPULATIONS_H
