#include "errno.h"
#include <string.h>

#include <stdio.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <unistd.h>
#include "../defines.h"
#include "../arg_parsing.h"
#include "../file_manipulations.h"
#include "compress.h"
int main(int argc, char* argv[]) {
    errno = 0;
//    struct file_buffer output;
//    struct file_map input;
    struct args args;
    parse_args(argc, argv, &args);
    FILE* file_input = fopen(args.input_filename, "r");
    printf("%s\n%d\n", strerror(errno), errno);
    int fd_input = fileno(file_input);
    FILE* file_output = fopen(args.output_filename, "w+");
    printf("%s\n%d\n", strerror(errno), errno);
    int fd_output = fileno(file_output);

    fd_compress(fd_output, fd_input, args.blocksize);

    fclose(file_input);
    fclose(file_output);
    debug("Hello");
}
