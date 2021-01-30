#pragma once
#include <libsystem/Common.h>
#include <libsystem/Result.h>
#include <libsystem/compression/Common.h>
#include <libutils/BitWriter.h>
#include <libutils/HashMap.h>
#include <libutils/Vector.h>

class Deflate
{
public:
    Deflate(unsigned int compression_level);

    Result perform(const Vector<uint8_t> &uncompressed, Vector<uint8_t> &compressed);

private:
    static void write_block_header(BitWriter &out_stream, BlockType block_type, bool final);
    static void write_uncompressed_block(uint8_t *data, uint16_t data_len, BitWriter &out_stream, bool final);
    static void write_uncompressed_blocks(const Vector<uint8_t> &in_data, BitWriter &out_stream, bool final);

    static Result compress_none(const Vector<uint8_t> &, Vector<uint8_t> &);

    Callback<Result(const Vector<uint8_t> &, Vector<uint8_t> &)> _compression_method;
    unsigned int _compression_level;
    unsigned int _min_size_to_compress;
};