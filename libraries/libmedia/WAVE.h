#pragma once

#include <libutils/Endian.h>

namespace media::wave
{

struct RIFFChunk
{
    char id[4];
    le_uint32_t size;
};

struct RIFF
{
    RIFFChunk chunk;

    char format[4];
};

struct FMT
{
    RIFFChunk chunk;

    le_uint16_t audio_format;
    le_uint16_t num_channel;
    le_uint32_t sample_rate;
    le_uint32_t byte_rate;
    le_uint16_t byte_per_block;
    le_uint16_t bits_per_sample;
};

struct DATA
{
    RIFFChunk chunk;
};

struct WAVE
{
    RIFF riff;
    FMT fmt;
    DATA data;
};

} // namespace media::wave
