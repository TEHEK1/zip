//
// Created by Амир Кашапов on 18.05.2024.
//
#include <assert.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <memory.h>
#include "file_manipulations.h"
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) < (b) ? (a) : (b))
extern int errno;
errno_t init_file_map(struct file_map* out, int fd, size_t blocksize, void* dst, int prot, off_t seek){
    struct stat fs;
    if (fstat(fd, &fs) == -1){
        return errno;
    }
    size_t length = fs.st_size;
    assert(length>seek);
    off_t pa_offset = (seek +  blocksize -  1) - (seek +blocksize - 1) % blocksize;
    pa_offset = seek & ~(sysconf(_SC_PAGE_SIZE) - 1);
    if(seek < blocksize){
        out->block_start = pa_offset;
        out->mapped_mem = mmap(dst, MIN(length - pa_offset, 3 * blocksize), prot, MAP_PRIVATE, fd, pa_offset);
    }
    else{
        out->block_start = pa_offset - blocksize;
        out->mapped_mem = mmap(dst, MIN(length - pa_offset, 3 * blocksize), prot, MAP_PRIVATE, fd, pa_offset - blocksize);
    }
    out->length = length;
    out->blocksize = blocksize;
    out->seek = seek;
    out->fd = fd;
    out->prot = prot;
    return  0;
}
errno_t init_file_buffer(struct file_buffer* out, int fd, size_t blocksize, void* dst, off_t seek){
    if(dst == NULL){
        out->mapped_mem = malloc(3 * blocksize);
    }
    else{
        out->mapped_mem = dst;
    }
    out->fd = fd;
    out->blocksize = blocksize;
    if(seek < blocksize){
        out->block_start = seek - seek%blocksize;
    }
    else{
        out->block_start = seek - seek%blocksize - blocksize;
    }
    out->seek = seek;
    return 0;
}
static errno_t update_file_map(struct file_map* out, off_t seek){
    off_t pa_offset = (seek +  out->blocksize -  1) - (seek + out->blocksize - 1) % out->blocksize;
    pa_offset = seek & ~(sysconf(_SC_PAGE_SIZE) - 1);
    if(pa_offset < out->blocksize){
        out->block_start = pa_offset;
        out->mapped_mem = mmap((void*)out->mapped_mem, MIN(out->length  - pa_offset, 3 * out->blocksize), out->prot, MAP_PRIVATE, out->fd, pa_offset);
    }
    else{
        out->block_start = pa_offset - out->blocksize;
        out->mapped_mem = mmap(out->mapped_mem, MIN(out->length - pa_offset, 3 * out->blocksize), out->prot, MAP_PRIVATE, out->fd, pa_offset - out->blocksize);
    }
    out->seek = seek;
    return 0;
}
static errno_t update_file_buffer(struct file_buffer* out, off_t seek){
    lseek(out->fd, out->block_start, L_SET);
    write(out->fd, out->mapped_mem, out->blocksize);
    if(out->block_start + 2 * out->blocksize <= seek) {
        out->block_start+=out->blocksize;
        memcpy(out->mapped_mem, out->mapped_mem + out->blocksize, out->blocksize);

    }
    else{
        out->block_start = seek - seek % out->blocksize - out->blocksize;
        lseek(out->fd, out->block_start, L_SET);
        read(out->fd, out->mapped_mem, out->blocksize);
    }
    out->seek = seek;
    return 0;
}
errno_t update_check_file_map(struct file_map* out){
    if(out->seek>=out->block_start + 2 * out->blocksize){
        return update_file_map(out, out->seek);
    }
    return 0;
}
errno_t update_check_file_buffer(struct file_buffer* out){
    if(out->seek>=out->block_start + 2 * out->blocksize){
        return update_file_buffer(out, out->seek);
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
char inline put_byte_file_buffer(struct file_buffer* output, char new_byte){
    update_check_file_buffer(output);
    output->mapped_mem[output->seek - output->block_start] = new_byte;
    output->seek++;
    update_check_file_buffer(output);
    return new_byte;
}
errno_t deinit_file_map(struct file_map* out){
    struct stat fs;
    if (fstat(out->fd, &fs) == -1){
        return errno;
    }
    munmap(out->mapped_mem, MIN(fs.st_size - out->block_start, 3 * out->blocksize) - out->blocksize);

    return 0;
}
errno_t deinit_file_buffer(struct file_buffer* out){
    write(out->fd, out->mapped_mem, MIN(out->seek - out->block_start, 3 * out->blocksize));
    free(out->mapped_mem);
    return 0;
}