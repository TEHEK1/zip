#include "compress_zlib.h"
int compress_zlib(struct file_map* output, struct file_map* input){
    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    int ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
    if (ret != Z_OK) {
        fprintf(stderr, "Error initializing zlib: %s\n", strm.msg);
        return 1;
    }

    int flush; // Variable to control flushing

    do {
        if((ret = update_check_file_map(input, input->seek))){
            return ret;
        }
        strm.avail_in = input->block_end - input->seek;
        flush = get_file_size(input->fd) <= input->block_end ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = (unsigned char*) input->mapped_mem + input->seek - input->block_start;
        do {
            if((ret = update_check_file_map(output, output->seek))){
                return ret;
            }
            strm.avail_out = output->block_end - output->seek;
            strm.next_out = (unsigned char*) output->mapped_mem + output->seek - output->block_start;
            ret = deflate(&strm, flush);
            if (ret == Z_STREAM_ERROR) {
                fprintf(stderr, "Error deflating data: %s\n", strm.msg);
                deflateEnd(&strm);
                return 1;
            }

            output->seek = output->block_end - strm.avail_out;
        } while (strm.avail_out == 0);
        input->seek = input->block_end - strm.avail_in;
    } while (flush != Z_FINISH);
    deflateEnd(&strm);
    return 0;
}