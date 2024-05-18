//
// Created by Амир Кашапов on 18.05.2024.
//
#include <argp.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "defines.h"
#include "argparsing.h"

const char *argp_program_version = "zip 0.1";
const char *argp_program_bug_address = "kasapovamir9@gmail.com";
static char doc[] = "Program for compressing files.";
static char args_doc[] = "zip [OPTIONS] filename [ARCHIVE.zip]";

static struct argp_option options[] = {
        {"bs", 'b', "SIZE", OPTION_ARG_OPTIONAL, "Block size (e.g., 4K, 16K, 1M). bs=4K by default. bs must be greater than 4K"},
        {"verbose", 'v', 0, 0, "Verbose output"}
};

static errno_t parse_opt(int key, const char *arg, struct argp_state *state) {
    struct args *args = state->input;

    switch (key) {
        case 'b':
            if (!arg)
                argp_usage(state);
            char** endptr;
            args->blocksize = strtol(arg, endptr, 0);
            if(*endptr == arg || (**endptr!='K' && **endptr!='M' && **endptr!='\0')){

            }
            break;
        case 'v':
            args->verbose = true;
            break;
        case ARGP_KEY_ARG:
            if (state->arg_num >= 2)
                argp_usage(state);
            args->args[state->arg_num] = arg;
            break;
        case ARGP_KEY_END:
            if (state->arg_num < 1)
                argp_usage(state);
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static void adjust_argument(struct args *args) {
    if (args->blocksize) {
        char *blocksize = args->blocksize;
        if(blocksize[0] == '=') {
            blocksize++;
        }
        if(blocksize[strlen(blocksize) - 1] == 'K') {
            blocksize[strlen(blocksize) - 1] = '\0';
            args->blocksize_int = atoi(blocksize) * 1024;
        } else if(blocksize[strlen(blocksize) - 1] == 'M') {
            blocksize[strlen(blocksize) - 1] = '\0';
            args->blocksize_int = atoi(blocksize) * 1024 * 1024;
        } else {
            args->blocksize_int = atoi(blocksize);
        }

        if(args->blocksize_int < 4096) {
            printf("Block size must be greater than 4K\n");
            exit(1);
        }
    }
    else{
        args->blocksize_int = 4096;
    }
}

static struct argp argp = {options, parse_opt, args_doc, doc};

void parse_args(int argc, char **argv, struct args *args) {
    args->blocksize = 4096;
    args->verbose = false;

    argp_parse(&argp, argc, argv, 0, 0, args);
    adjust_argument(args);

    debug("FILENAME: %s\n", args->args[0]);
    if (args->args[1])
        debug("ARCHIVE: %s\n", args->args[1])
    if (args->blocksize)
        DEBUG("Block size: %d\n", args->blocksize_int)
    if (args->verbose)
        DEBUG("Verbose output enabled\n")
}
