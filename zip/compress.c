//
// Created by Амир Кашапов on 20.05.2024.
//

#include "compress.h"
#include "compress_zlib.h"
#include "../file_manipulations.h"
static errno_t file_map_compress(struct file_map* output_file_map, struct file_map* input_file_map){
    compress_zlib(output_file_map, input_file_map);
    return 0;
}
errno_t fd_compress(int output_fd, int input_fd, size_t blocksize){
    struct file_map output_file_map, input_file_map;
    init_file_map(&output_file_map, output_fd, blocksize, NULL, PROT_WRITE | PROT_READ, 0);
    init_file_map(&input_file_map, input_fd, blocksize, NULL, PROT_READ, 0);
    file_map_compress(&output_file_map, &input_file_map);
    deinit_file_map(&output_file_map);
    deinit_file_map(&input_file_map);
    return 0;
}