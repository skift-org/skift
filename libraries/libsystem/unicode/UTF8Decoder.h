#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/unicode/Codepoint.h>

typedef void (*UTF8DecoderCallback)(Codepoint codepoint, void *args);

typedef struct
{
    bool is_decoding;
    Codepoint current_decoding;
    int width_decoding;

    void *callback_args;
    UTF8DecoderCallback callback;
} UTF8Decoder;

UTF8Decoder *utf8decoder_create(UTF8DecoderCallback callback, void *args);

void utf8decoder_destroy(UTF8Decoder *decoder);

void utf8decoder_write(UTF8Decoder *decoder, uint8_t byte);

void utf8decoder_flush(UTF8Decoder *decoder);