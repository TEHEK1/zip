//
// Created by Амир Кашапов on 18.05.2024.
//
#include <sys/mman.h>
#include <assert.h>
#include "file_manipulations.h"
#include "lz77_decompression.h"
static void lz77_output_backref_by_file_buffer(struct file_buffer* output, size_t dist, size_t len){
    update_check_file_buffer(output);
    for (size_t i = 0; i < len; i++) {
        output->mapped_mem[output->seek] = output->mapped_mem[output->seek - dist];
        output->seek++;
    }
    update_check_file_buffer(output);
}
static inline void lz77_output_lit_by_file_buffer(struct file_buffer* output, char lit)
{
    put_byte_file_buffer(output, lit);
}
errno_t lz77_decompress_by_file_map(struct file_buffer* output, struct file_map* input){
    char cur_byte;
    while(input->seek < input->length){
        cur_byte = get_byte_file_map(input);
        if(cur_byte != EOF){
            if(cur_byte == '('){
                char str_dist[10];
                for(int i = 0;i <10;i++)str_dist[i] = 0;
                for(int i = 0;(cur_byte = get_byte_file_map(input)) != ',' && i <10; i++)str_dist[i] = cur_byte;
                size_t dist = strtol(str_dist, NULL, 10);
                char str_len[10];
                for(int i = 0;i <10;i++)str_len[i] = 0;
                for(int i = 0;(cur_byte = get_byte_file_map(input)) != ')' && i <10; i++)str_len[i] = cur_byte;
                size_t len = strtol(str_len, NULL, 10);
                lz77_output_backref_by_file_buffer(output, dist, len);

            }
            else{
                lz77_output_lit_by_file_buffer(output, cur_byte);
            }
        }
    }
    return 0;
}
errno_t lz77_decompress_by_fd(int output_fd, int input_fd, size_t blocksize) {
    int err = 0;
    struct file_map input;
    struct file_buffer output;
    if((err = init_file_map(&input, input_fd, blocksize, NULL, PROT_READ, 0))){
        return err;
    }
    if((err = init_file_buffer(&output, output_fd, blocksize, NULL, 0))){
        deinit_file_map(&input);
        return err;
    }
    lz77_decompress_by_file_map(&output, &input);
    deinit_file_map(&input);
    deinit_file_buffer(&output);

    return 0;
}