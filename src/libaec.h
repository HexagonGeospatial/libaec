/**
 * @file libaec.h
 *
 * @author Mathis Rosenhauer, Deutsches Klimarechenzentrum
 * @author Moritz Hanke, Deutsches Klimarechenzentrum
 * @author Joerg Behrens, Deutsches Klimarechenzentrum
 * @author Luis Kornblueh, Max-Planck-Institut fuer Meteorologie
 *
 * @section LICENSE
 * Copyright 2012 - 2014
 *
 * Mathis Rosenhauer,                 Luis Kornblueh
 * Moritz Hanke,
 * Joerg Behrens
 *
 * Deutsches Klimarechenzentrum GmbH  Max-Planck-Institut fuer Meteorologie
 * Bundesstr. 45a                     Bundesstr. 53
 * 20146 Hamburg                      20146 Hamburg
 * Germany                            Germany
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @section DESCRIPTION
 *
 * Adaptive Entropy Coding library
 *
 */

#ifndef LIBAEC_H
#define LIBAEC_H 1

#include <stddef.h>

#if _WIN32 && BUILD_SHARED_LIBS
#  ifdef DLL_EXPORT
#    define AEC_SCOPE __declspec(dllexport)
#  else
#    define AEC_SCOPE extern __declspec(dllimport)
#  endif
#else
#  define AEC_SCOPE extern
#endif

struct internal_state;

struct aec_stream {
    const unsigned char *next_in;
    size_t avail_in;            /* number of bytes available at
                                 * next_in
                                 */
    size_t total_in;            /* total number of input bytes read so
                                 * far
                                 */
    unsigned char *next_out;
    size_t avail_out;           /* remaining free space at next_out */
    size_t total_out;           /* total number of bytes output so far */
    int bits_per_sample;        /* resolution in bits per sample (n =
                                 * 1, ..., 32)
                                 */
    int block_size;             /* block size in samples */
    int rsi;                    /* Reference sample interval, the
                                 * number of Coded Data Sets between
                                 * consecutive reference samples (up
                                 * to 4096).
                                 */
    int flags;

    struct internal_state *state;
};

/* Sample data description flags */
#define AEC_DATA_SIGNED 1        /* Samples are signed. Telling libaec
                                  * this results in a slightly better
                                  * compression ratio. Default is
                                  * unsigned.
                                  */
#define AEC_DATA_3BYTE 2         /* 24 bit samples are coded in 3 bytes */
#define AEC_DATA_MSB 4           /* Samples are stored with their most
                                  * significant bit first. This has
                                  * nothing to do with the endianness
                                  * of the host. Default is LSB.
                                  */
#define AEC_DATA_PREPROCESS 8    /* Set if preprocessor should be used */

#define AEC_RESTRICTED 16        /* Use restricted set of code options */
#define AEC_PAD_RSI 32           /* Pad RSI to byte boundary. Only for
                                  * decoding CCSDS sample data. */

/* Return codes of library functions */
#define AEC_OK            0
#define AEC_CONF_ERROR   (-1)
#define AEC_STREAM_ERROR (-2)
#define AEC_DATA_ERROR   (-3)
#define AEC_MEM_ERROR    (-4)

/* Options for flushing */
#define AEC_NO_FLUSH      0      /* Do not enforce output
                                  * flushing. More input may be
                                  * provided with later calls. So far
                                  * only relevant for encoding.
                                  */
#define AEC_FLUSH         1      /* Flush output and end encoding. The
                                  * last call to aec_encode() must set
                                  * AEC_FLUSH to drain all output.
                                  *
                                  * It is not possible to continue
                                  * encoding of the same stream after it
                                  * has been flushed because the last byte
                                  * may be padded with fill bits.
                                  */

/* Streaming encoding and decoding functions */
AEC_SCOPE int aec_encode_init(struct aec_stream *strm);
AEC_SCOPE int aec_encode(struct aec_stream *strm, int flush);
AEC_SCOPE int aec_encode_end(struct aec_stream *strm);

AEC_SCOPE int aec_decode_init(struct aec_stream *strm);
AEC_SCOPE int aec_decode(struct aec_stream *strm, int flush);
AEC_SCOPE int aec_decode_end(struct aec_stream *strm);

/* Utility functions for encoding or decoding a memory buffer. */
AEC_SCOPE int aec_buffer_encode(struct aec_stream *strm);
AEC_SCOPE int aec_buffer_decode(struct aec_stream *strm);

#endif /* LIBAEC_H */
