#include <sys/stat.h>
#include <unistd.h>
#include "file_manipulations.h"

#define MIN(a,b) ((a) < (b) ? (a) : (b))
extern int errno;

off_t get_file_size(int fd){
    struct stat fs;
    if (fstat(fd, &fs) == -1){
        return errno;
    }
    return fs.st_size;
}
static off_t get_align(off_t seek, size_t blocksize){
    off_t pa_offset = (off_t)(seek - seek % blocksize);
    return pa_offset & ~(sysconf(_SC_PAGE_SIZE) - 1);
}
int init_file_map(struct file_map* out, int fd, size_t blocksize, void* dst, int prot, int flags, off_t seek){
    out->fd = fd;
    out->blocksize = blocksize = get_align((off_t)blocksize, sysconf(_SC_PAGE_SIZE));
    out->seek = seek;
    out->prot = prot;
    out->flags = flags;
    off_t size = get_file_size(out->fd);
    out->length = out->seek;
    off_t pa_offset = get_align(out->seek, out->blocksize);
    if(size <= out->seek){
        if(ftruncate(out->fd, pa_offset + (off_t)out->blocksize) < 0){
            return errno;
        }
        size = pa_offset + (off_t) out->blocksize;
    }
    out->block_end = MIN(size, (off_t) (pa_offset + out->blocksize));
    out->block_start = pa_offset;
    if((out->mapped_mem = (char*)mmap(dst, out->block_end - out->block_start, out->prot, out->flags, out->fd, out->block_start))==MAP_FAILED){
        return errno;
    }
    return  0;
}
static int update_file_map(struct file_map* out, off_t seek){
    size_t size = get_file_size(out->fd);
    off_t pa_offset = get_align(seek, out->blocksize);
    out->seek = seek;
    if(out->prot & PROT_WRITE && out->flags & MAP_SHARED){
        if(msync(out->mapped_mem, out->block_end - out->block_start, MS_SYNC) < 0){
            return errno;
        }
    }
    if(out->length < out->seek){
        out->length = out->seek;
    }
    if((off_t)size <= out->seek && out->prot & PROT_WRITE && out->flags & MAP_SHARED){
        if(ftruncate(out->fd, pa_offset + (off_t) out->blocksize) < 0){
            return errno;
        }
        size = pa_offset + out->blocksize;
    }
    else if((off_t)size <= out->seek){
        return EACCES;
    }
    out->block_end = MIN(size, (size_t)pa_offset + out->blocksize);
    out->block_start = pa_offset;
    if(munmap(out->mapped_mem, out->block_end - out->block_start) < 0){
        return errno;
    }
    if((out->mapped_mem = (char*)mmap(out->mapped_mem, out->block_end - out->block_start, out->prot, out->flags, out->fd, out->block_start))==MAP_FAILED){
        return errno;
    }
    return 0;
}
int update_check_file_map(struct file_map* out, off_t length){
    out->seek = length;
    if(out->seek > out->length){
        out->length = out->seek;
    }
    if(out->seek >= out->block_end){
        return update_file_map(out, out->seek);
    }
    return 0;
}
int deinit_file_map(struct file_map* out){
    if(update_check_file_map(out, out->seek)) {
        return EOF;
    }
    if(out->prot & PROT_WRITE && out->flags & MAP_SHARED) {
        if(msync(out->mapped_mem, out->block_end - out->block_start, MS_SYNC) < 0){
            return errno;
        }
        if(ftruncate(out->fd, out->length) < 0){
            return errno;
        }
    }
    if(munmap(out->mapped_mem, out->block_end - out->block_start) < 0){
        return errno;
    }
    return 0;
}