//
// Created by Амир Кашапов on 18.05.2024.
//
#include <argp.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "defines.h"
#include "arg_parsing.h"

const char *argp_program_version = "zip 0.1";
const char *argp_program_bug_address = "kasapovamir9@gmail.com";
static char doc[] = "Program for compressing files.";
static char args_doc[] = "zip [OPTIONS] filename [ARCHIVE.zip]";

static struct argp_option options[] = {
        {"bs", 'b', "SIZE", OPTION_ARG_OPTIONAL, "Block size (e.g., 4K, 16K, 1M). bs=4K by default. bs must be greater than 4K"},
        {"verbose", 'v', 0, 0, "Verbose output"},
        {0}
};
static errno_t parse_blocksize(struct args* args, char *arg, struct argp_state *state){
    if (!arg)
        argp_error(state, "Must be block size");
    char* endptr =  (char*)1;
    args->blocksize = strtol(arg, &endptr, 0);
    if(endptr == arg || (*endptr!='K' && *endptr!='M' && *endptr!='\0')){
        argp_error(state, "Error in specifiing block size");
    }
    switch (*endptr) {
        case 'M':
            args->blocksize*=1024;
        case 'K':
            args->blocksize*=1024;
            break;
    }
    if(args->blocksize < 4096) {
        argp_error(state, "Block size must be at least 4K");
    }
    return 0;
}
static errno_t parse_argument(struct args* args, char *arg, struct argp_state *state){
    switch (state->arg_num) {
        case 0:
            args->input_filename = malloc(strlen(arg) + 1);
            if(!args->input_filename) {return ENOMEM;}
            memcpy(args->input_filename, arg, strlen(arg) + 1);
            break;
        case 1:
            args->output_filename = malloc(strlen(arg) + 1);
            if(!args->output_filename) {return ENOMEM;}
            memcpy(args->output_filename, arg, strlen(arg) + 1);
            break;
        default:
            argp_error(state, "Too much args");
    }
    return 0;
}
static errno_t parse_end(struct args* args, struct argp_state *state){
    switch (state->arg_num) {
        case 0:
            argp_error(state, "Must be at least one argument");
            break;
        case 1:
            args->output_filename = malloc(strlen(args->input_filename)+5);
            if(!args->output_filename) {return ENOMEM;}
            memcpy(args->output_filename,args->input_filename, strlen(args->input_filename));
            strcat(args->output_filename, ".zip");
            break;
    }
    return 0;
}

static errno_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct args *args = state->input;
    int err = 0;
    switch (key) {
        case 'b':
            if((err = parse_blocksize(args, arg, state))){
                return err;
            }
            break;
        case 'v':
            args->verbose = true;
            break;
        case ARGP_KEY_ARG:
            if((err = parse_argument(args, arg, state))){
                return err;
            }
            parse_argument(args, arg, state);
            break;
        case ARGP_KEY_END:
            if((err = parse_end(args, state))){
                return err;
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}


static struct argp argp = {options, parse_opt, args_doc, doc};

errno_t parse_args(int argc, char **argv, struct args *args) {
    args->blocksize = 4096;
    args->verbose = false;
    int err = 0;
    if((err = argp_parse(&argp, argc, argv, 0, 0, args))){
        fprintf(stderr, "%s", strerror(err));
        return err;
    }
    printf("FILENAME: %s\n", args->input_filename);
    printf("ARCHIVE: %s\n", args->output_filename);
        printf("Block size: %lu\n", args->blocksize);
    if (args->verbose)
        printf("Verbose output enabled\n");
    return 0;
}
