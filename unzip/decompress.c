//
// Created by Амир Кашапов on 20.05.2024.
//

#include "decompress.h"
#include "decompress_zlib.h"
#include "../file_manipulations.h"
static errno_t file_map_decompress(struct file_map* output_file_map, struct file_map* input_file_map){
    decompress_zlib(output_file_map, input_file_map);
    return 0;
}
errno_t fd_decompress(int output_fd, int input_fd, size_t blocksize){
    struct file_map output_file_map, input_file_map;
    init_file_map(&output_file_map, output_fd, blocksize, NULL, PROT_WRITE | PROT_READ, 0);
    init_file_map(&input_file_map, input_fd, blocksize, NULL, PROT_READ, 0);
    file_map_decompress(&output_file_map, &input_file_map);
    deinit_file_map(&output_file_map);
    deinit_file_map(&input_file_map);
    return 0;
}