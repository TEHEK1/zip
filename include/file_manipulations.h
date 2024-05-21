#ifndef KASP_FILE_MANIPULATIONS_H
#define KASP_FILE_MANIPULATIONS_H
#include <errno.h>
#include <stdio.h>
#include <sys/mman.h>

struct file_map{
    char* mapped_mem;
    off_t seek;
    off_t block_start;
    off_t block_end;
    off_t length;
    size_t blocksize;
    int prot;
    int flags;
    int fd;
};
off_t get_file_size(int fd);
int init_file_map(struct file_map* out, int fd, size_t blocksize, void* dst, int prot, int flags, off_t seek);
int update_check_file_map(struct file_map* out);
int deinit_file_map(struct file_map* out);
#endif //KASP_FILE_MANIPULATIONS_H
