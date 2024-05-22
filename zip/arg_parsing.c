#include <argp.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "arg_parsing.h"


const char *argp_program_version = "zip 1.0";
const char *argp_program_bug_address = "kasapovamir9@gmail.com";
static char doc[] = "Program for compressing files.";
static char args_doc[] = "zip [OPTIONS] filename [archive.zip]";

static struct argp_option options[] = {
        {"bs", 'b', "SIZE", OPTION_ARG_OPTIONAL, "Block size (e.g., 16K, 1M).Bigger values will increase speed", 0},
        {"verbose", 'v', 0, 0, "Verbose output", 0},
        {0}
};
static int blocksize_parse(struct arg_values* args, char *arg, struct argp_state *state){
    if (!arg) {
        argp_error(state, "Must be block size");
        return EWRONG_BLOCK_SIZE;
    }
    char* end_ptr =  (char*)1;
    args->blocksize = strtol(arg, &end_ptr, 0);
    if(end_ptr == arg || (*end_ptr!='K' && *end_ptr!='M' && *end_ptr!='\0')){
        argp_error(state, "Error in specifying block size");
        return EWRONG_BLOCK_SIZE;
    }
    switch (*end_ptr) {
        case 'M':
            args->blocksize*=1024;
        case 'K':
            args->blocksize*=1024;
            break;
        case '\0':
            break;
        default:
            return EWRONG_BLOCK_SIZE;
    }
    if(args->blocksize < 16384) {
        argp_error(state, "Block size must be at least 16K");
        return EWRONG_BLOCK_SIZE;
    }
    return 0;
}
static int argument_parse(struct arg_values* args, char *arg, struct argp_state *state){
    switch (state->arg_num) {
        case 0:
            args->input_filename = (char*)malloc(strlen(arg) + 1);
            if(!args->input_filename) {return ENOMEM;}
            memcpy(args->input_filename, arg, strlen(arg) + 1);
            break;
        case 1:
            args->output_filename = (char*)malloc(strlen(arg) + 1);
            if(!args->output_filename) {return ENOMEM;}
            memcpy(args->output_filename, arg, strlen(arg) + 1);
            break;
        default:
            argp_error(state, "Too much args");
            return ETOO_MUCH_ARGUMENTS;
    }
    return 0;
}
static int end_parse(__attribute__((unused)) struct arg_values* args, struct argp_state *state){
    switch (state->arg_num) {
        case 0:
            argp_error(state, "Must be at least one argument");
            return ENOT_ENOUGH_ARGUMENTS;
    }
    return 0;
}

static int option_parse(int key, char *arg, struct argp_state *state) {
    struct arg_values *args = (struct arg_values *)state->input;
    int err;
    switch (key) {
        case 'b':
            if((err = blocksize_parse(args, arg, state))){
                return err;
            }
            break;
        case 'v':
            args->verbose = true;
            break;
        case ARGP_KEY_ARG:
            if((err = argument_parse(args, arg, state))){
                return err;
            }
            //argument_parse(args, arg, state);
            break;
        case ARGP_KEY_END:
            if((err = end_parse(args, state))){
                return err;
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}




int arg_values_parse(int argc, char **argv, struct arg_values *args) {
    static struct argp argp = {options, option_parse, args_doc, doc, 0, 0, 0};
    args->blocksize = 16384;
    args->verbose = false;
    args->output_filename = "archive.zip";
    int err;
    if((err = argp_parse(&argp, argc, argv, 0, 0, args))){
        fprintf(stderr, "%s", strerror(err));
        return err;
    }
    if (args->verbose){
        printf("Verbose output enabled\n");
        printf("Input filename:%s\n", args->input_filename);
        printf("Output filename:%s\n", args->output_filename);
        printf("Blocksize:%lu\n", args->blocksize);
    }
    return 0;
}
