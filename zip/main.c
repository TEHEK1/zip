#include <errno.h>
#include <string.h>
#include <stdio.h>
#include "arg_parsing.h"
#include "compress.h"

int main(int argc, char* argv[]) {
    struct arg_values args;
    arg_values_parse(argc, argv, &args);
    FILE* file_input = fopen(args.input_filename, "r");
    if(!file_input) {
        printf("%s\n%d\n", strerror(errno), errno);
    }
    int fd_input = fileno(file_input);
    FILE* file_output = fopen(args.output_filename, "w+");
    if(!file_output) {
        printf("%s\n%d\n", strerror(errno), errno);
    }
    int fd_output = fileno(file_output);

    if(fd_compress(fd_output, fd_input, args.blocksize)){
        printf("%s\n%d\n", strerror(errno), errno);
    }

    fclose(file_input);
    fclose(file_output);
}
