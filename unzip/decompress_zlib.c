//
// Created by Амир Кашапов on 20.05.2024.
//

#include "decompress_zlib.h"
errno_t decompress_zlib(struct file_map* output, struct file_map* input){
    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    int ret = inflateInit(&strm);
    if (ret != Z_OK) {
        fprintf(stderr, "Error initializing zlib: %s\n", strm.msg);
        return 1;
    }

    do {

        if (strm.avail_in == 0)
            break;

        strm.next_in = (unsigned char*) input->mapped_mem + input->seek - input->block_start;

        do {
            strm.avail_out = output->blocksize;
            strm.next_out = (unsigned char*) output->mapped_mem + output->seek - output->block_start;
            ret = inflate(&strm, Z_NO_FLUSH);
            switch (ret) {
                case Z_NEED_DICT:
                    ret = Z_DATA_ERROR;
                    /* fall through */
                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                    fprintf(stderr, "Error inflating data: %s\n", strm.msg);
                    inflateEnd(&strm);
                    return 1;
            }
            output->seek = output->block_end - strm.avail_out;
        } while (strm.avail_out == 0);
        input->seek = input->block_end - strm.avail_in;
    } while (ret != Z_STREAM_END);

    inflateEnd(&strm);
    return 0;
}