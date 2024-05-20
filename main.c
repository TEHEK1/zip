#include "errno.h"
#include <string.h>

#include <stdio.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <unistd.h>
#include "defines.h"
#include "arg_parsing.h"
#include "file_manipulations.h"
#include "lz77_decompression.h"
int main(int argc, char* argv[]) {
//    struct file_buffer output;
//    struct file_map input;
    FILE* file_input = fopen("input.txt", "r");
    printf("%s\n%d\n", strerror(errno), errno);
    int fd_input = fileno(file_input);
    FILE* file_output = fopen("output.txt", "w+");
    printf("%s\n%d\n", strerror(errno), errno);
    int fd_output = fileno(file_output);

    lz77_decompress_by_fd(fd_output, fd_input, 1024);


    fclose(file_input);
    fclose(file_output);
    debug("Hello");
}
