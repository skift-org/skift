#pragma once
#include <libcompression/Common.h>
#include <libio/BitWriter.h>
#include <libio/Reader.h>
#include <libsystem/Common.h>
#include <libsystem/Result.h>

namespace Compression
{

class Deflate
{
private:
    unsigned int _compression_level;
    unsigned int _min_size_to_compress;

    // Compression modes
    static Result compress_none(IO::Reader &uncompressed, IO::Writer &compressed);

    // Write functions
    static Result write_uncompressed_blocks(IO::Reader &in_data, IO::BitWriter &out_writer, bool final);
    static void write_block_header(IO::BitWriter &out_writer, BlockType block_type, bool final);
    static void write_uncompressed_block(const uint8_t *block_data, size_t block_len, IO::BitWriter &out_writer, bool final);

public:
    Deflate(unsigned int compression_level);

    Result perform(IO::Reader &uncompressed, IO::Writer &compressed);
};

} // namespace Compression