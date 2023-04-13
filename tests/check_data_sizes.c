#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "check_aec.h"

#define BUF_SIZE 1024 * 3

int check_block_sizes(struct test_state *state)
{
    for (int bs = 8; bs <= 64; bs *= 2) {
        int status;
        state->strm->block_size = bs;
        state->strm->rsi = (int)(state->buf_len
                                 / (bs * state->bytes_per_sample));

        status = encode_decode_large(state);
        if (status)
            return status;
    }
    return 0;
}

int check_rsi(struct test_state *state)
{
    int status;
    int size = state->bytes_per_sample;

    for (unsigned char *tmp = state->ubuf;
         tmp < state->ubuf + state->buf_len;
         tmp += 2 * state->bytes_per_sample) {
        state->out(tmp, state->xmax, size);
        state->out(tmp + size, state->xmin, size);
    }

    status = check_block_sizes(state);
    if (status)
        return status;

    return 0;
}

int check_data_size(struct test_state *state)
{
    int status;

    for (int bps = 1; bps <= state->bytes_per_sample << 3; bps++) {
        printf("Checking %i bytes, %i bps ... ", state->bytes_per_sample, bps);
        if (bps > 16)
            state->id_len = 5;
        else if (bps > 8)
            state->id_len = 4;
        else
            state->id_len = 3;
        state->strm->bits_per_sample = bps;
        state->xmax = (1ULL << bps) - 1;

        status = check_rsi(state);
        if (status)
            return status;
        printf ("%s\n", CHECK_PASS);
    }
    return 0;
}

static void out_lsb(unsigned char *dest, unsigned long long int val, int size)
{
    for (int i = 0; i < size; i++)
        dest[i] = (unsigned char)(val >> (8 * i));
}

int main (void)
{
    int status;
    struct aec_stream strm;
    struct test_state state;

    state.dump = 0;
    state.buf_len = state.ibuf_len = BUF_SIZE;
    state.cbuf_len = 2 * BUF_SIZE;

    state.ubuf = (unsigned char *)malloc(state.buf_len);
    state.cbuf = (unsigned char *)malloc(state.cbuf_len);
    state.obuf = (unsigned char *)malloc(state.buf_len);

    if (!state.ubuf || !state.cbuf || !state.obuf) {
        printf("Not enough memory.\n");
        status = 99;
        goto DESTRUCT;
    }

    strm.flags = AEC_DATA_PREPROCESS | AEC_DATA_DOUBLEWORD;
    state.strm = &strm;
    state.out = out_lsb;
    state.xmin = 0;

    state.bytes_per_sample = 4;
    status = check_data_size(&state);
    if (status)
        goto DESTRUCT;
    state.bytes_per_sample = 2;
    status = check_data_size(&state);

DESTRUCT:
    if (state.ubuf)
        free(state.ubuf);
    if (state.cbuf)
        free(state.cbuf);
    if (state.obuf)
        free(state.obuf);

    return status;
}
