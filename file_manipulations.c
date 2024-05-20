//
// Created by Амир Кашапов on 18.05.2024.
//
#include <assert.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <unistd.h>
#include <memory.h>
#include "file_manipulations.h"
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) < (b) ? (a) : (b))
extern int errno;
size_t get_file_size(int fd){
    struct stat fs;
    if (fstat(fd, &fs) == -1){
        return errno;
    }
    return fs.st_size;
}
static off_t get_align(off_t seek, size_t blocksize){
    off_t pa_offset = (seek +  blocksize -  1) - (seek +blocksize - 1) % blocksize;
    return seek & ~(sysconf(_SC_PAGE_SIZE) - 1);
}
errno_t init_file_map(struct file_map* out, int fd, size_t blocksize, void* dst, int prot, off_t seek){
    out->fd = fd;
    out->blocksize = blocksize;
    out->seek = seek;
    out->prot = prot;
    size_t size = get_file_size(out->fd);
    out->length = size;
    off_t pa_offset = get_align(out->seek, out->blocksize);
    if(out->length <= out->seek){
        out->length = out->seek;
        ftruncate(out->fd, pa_offset + 2 * out->blocksize);
        size = pa_offset + 2 * out->blocksize;
    }
    out->block_end = MIN(size, pa_offset + 2 * out->blocksize);
    if(pa_offset < out->blocksize){
        out->block_start = pa_offset;
    }
    else{
        out->block_start = pa_offset - out->blocksize;
    }
    out->mapped_mem = mmap(dst, out->block_end - out->block_start, out->prot, MAP_SHARED, out->fd, out->block_start);
    return  0;
}
static errno_t update_file_map(struct file_map* out, off_t seek){

    size_t size = get_file_size(out->fd);
    off_t pa_offset = get_align(seek, out->blocksize);
    out->seek = seek;
    if(out->prot & PROT_WRITE){
        msync(out->mapped_mem, out->block_end - out->block_start, MS_SYNC);
        munmap(out->mapped_mem, out->block_end - out->block_start);
    }
    if(out->length < out->seek && out->seek < out->block_end){
        out->length = out->seek;
    }
    else if(out->length < out->seek && out->prot & PROT_WRITE){
        out->length = out->seek;
        ftruncate(out->fd, pa_offset + 2 * out->blocksize);
        size = pa_offset + 2 * out->blocksize;
    }
    else if(out->length < out->seek){
        return EACCES;
    }
    out->block_end = MIN(size, pa_offset + 2 * out->blocksize);
    if(pa_offset < out->blocksize){
        out->block_start = pa_offset;
    }
    else{
        out->block_start = pa_offset - out->blocksize;
    }
    out->mapped_mem = mmap(out->mapped_mem, out->block_end - out->block_start, out->prot, MAP_SHARED, out->fd, out->block_start);
    return 0;
}
errno_t update_check_file_map(struct file_map* out){
    if(out->seek > out->length){
        out->length = out->seek;
    }
    if(out->seek + out->blocksize>=out->block_end && out->seek >= out->block_start + out->blocksize){
        return update_file_map(out, out->seek);
    }
    return 0;
}
char inline get_byte_file_map(struct file_map* input){
    if(input->seek>= input->length){
        return EOF;
    }
    update_check_file_map(input);
    char result = input->mapped_mem[input->seek - input->block_start];
    input->seek++;
    update_check_file_map(input);
    return result;
}
char inline put_byte_file_map(struct file_map* output, char new_byte){
    update_check_file_map(output);
    output->mapped_mem[output->seek - output->block_start] = new_byte;
    output->seek++;
    update_check_file_map(output);
    return new_byte;
}
errno_t deinit_file_map(struct file_map* out){
    size_t size = get_file_size(out->fd);
    if(out->prot & PROT_WRITE) {
        int dbg = msync(out->mapped_mem, out->block_end - out->block_start, MS_SYNC);
        ftruncate(out->fd, out->length);
    }
    munmap(out->mapped_mem, MIN(size - out->block_start, out->block_start?3 * out->blocksize:2 * out->blocksize));
    return 0;
}