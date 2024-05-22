#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <unistd.h>
#include "arg_parsing.h"
#include "compress.h"
void SEH_handler(int sig) {//Checking if launched under debug by checking trap sygnal
    if (sig == SIGTRAP) {
        exit(1);
    }
}
static int is_ptraced_proc() {//Checking if launched under debug by checking status
    FILE *fp = fopen("/proc/self/status", "r");
    if (!fp) {
        return -1;
    }
    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "TracerPid:") && strtol(line + 10, NULL, 10) != 0) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

static int is_ptraced_syscall() {//Checking if launched under debug by parent PID
    pid_t ppid = syscall(__NR_getppid);
    return ppid != getppid();
}
static int is_trap_not_captured(){
    volatile int a = 1;
    return a;
}
void anti_debug() {//Checking if launched under debug

    if (is_ptraced_proc() > 1) {
        fprintf(stderr, "Debug found ptrace proc\n");
        exit(1);
    }

    if (is_ptraced_syscall()) {
        fprintf(stderr, "Debug found ptrace syscall\n");
        exit(1);
    }
    signal(SIGTRAP, SEH_handler);
    is_trap_not_captured();
}
int main(int argc, char* argv[]) {
    struct arg_values args;
    arg_values_parse(argc, argv, &args);
    anti_debug();

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
