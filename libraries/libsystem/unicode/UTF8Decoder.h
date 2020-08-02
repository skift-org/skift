#pragma once

#include <libsystem/unicode/Codepoint.h>

typedef void (*UTF8DecoderCallback)(void *target, Codepoint codepoint);

struct UTF8Decoder
{
    bool is_decoding;
    Codepoint current_decoding;
    int width_decoding;

    void *target;
    UTF8DecoderCallback callback;
};

UTF8Decoder *utf8decoder_create(void *target, UTF8DecoderCallback callback);

void utf8decoder_destroy(UTF8Decoder *decoder);

void utf8decoder_write(UTF8Decoder *decoder, uint8_t byte);

void utf8decoder_flush(UTF8Decoder *decoder);
