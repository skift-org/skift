#pragma once
#include <libsystem/Common.h>
#include <libsystem/Result.h>
#include <libcompression/Common.h>
#include <libutils/Callback.h>

namespace IO
{
class BitWriter;
class Reader;
class Writer;
} // namespace IO

class Deflate
{
public:
    Deflate(unsigned int compression_level);

    Result perform(IO::Reader &uncompressed, IO::Writer &compressed);

private:
    static void write_block_header(IO::BitWriter &out_stream, BlockType block_type, bool final);
    static void write_uncompressed_block(Reader &in_data, uint16_t block_len, IO::BitWriter &out_stream, bool final);
    static void write_uncompressed_blocks(Reader &in_data, IO::BitWriter &out_stream, bool final);

    static Result compress_none(Reader &, Writer &);

    Callback<Result(Reader &, Writer &)> _compression_method;
    unsigned int _compression_level;
    unsigned int _min_size_to_compress;
};