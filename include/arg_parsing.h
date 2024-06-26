#ifndef KASP_ARG_PARSING_H
#define KASP_ARG_PARSING_H
#include <errno.h>
#include <stddef.h>
#define EWRONG_BLOCK_SIZE 150
#define ENOT_ENOUGH_ARGUMENTS 151
#define ETOO_MUCH_ARGUMENTS 152
struct arg_values{
    size_t blocksize;
    char verbose;
    char* input_filename;
    char* output_filename;
};
int arg_values_parse(int argc, char **argv, struct arg_values *args);
#endif //KASP_ARG_PARSING_H
