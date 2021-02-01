#pragma once
#include <libsystem/Common.h>
#include <libsystem/Result.h>
#include <libsystem/compression/Common.h>
#include <libsystem/io/BitWriter.h>
#include <libutils/Callback.h>

class Reader;
class Writer;
class Deflate
{
public:
    Deflate(unsigned int compression_level);

    Result perform(Reader &uncompressed, Writer &compressed);

private:
    static void write_block_header(BitWriter &out_stream, BlockType block_type, bool final);
    static void write_uncompressed_block(Reader &in_data, uint16_t block_len, BitWriter &out_stream, bool final);
    static void write_uncompressed_blocks(Reader &in_data, BitWriter &out_stream, bool final);

    static Result compress_none(Reader &, Writer &);

    Callback<Result(Reader &, Writer &)> _compression_method;
    unsigned int _compression_level;
    unsigned int _min_size_to_compress;
};