#include "compress.h"
#include "compress_zlib.h"
#include "file_manipulations.h"

static int file_map_compress(struct file_map* output_file_map, struct file_map* input_file_map){
    return compress_zlib(output_file_map, input_file_map);
}
int fd_compress(int output_fd, int input_fd, size_t blocksize){
    int err;
    struct file_map output_file_map, input_file_map;
    if((err = init_file_map(&output_file_map, output_fd, blocksize, NULL, PROT_WRITE | PROT_READ, MAP_SHARED, 0))){
        return err;
    }
    if((err = init_file_map(&input_file_map, input_fd, blocksize, NULL, PROT_READ, MAP_PRIVATE, 0))){
        deinit_file_map(&output_file_map);
        return err;
    }
    err = file_map_compress(&output_file_map, &input_file_map);
    deinit_file_map(&output_file_map);
    deinit_file_map(&input_file_map);
    return err;
}