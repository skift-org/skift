#pragma once
#include <libcompression/Common.h>
#include <libio/BitWriter.h>
#include <libio/Reader.h>
#include <libsystem/Common.h>
#include <libsystem/Result.h>
#include <libutils/Callback.h>

namespace Compression
{

class Deflate
{
private:
    unsigned int _compression_level;
    unsigned int _min_size_to_compress;

    // Compression modes
    static Result compress_none(IO::SeekableReader auto &uncompressed, IO::Writer &compressed)
    {
        IO::BitWriter bit_writer(compressed);
        write_uncompressed_blocks(uncompressed, bit_writer, true);

        return Result::SUCCESS;
    }

    // Write functions
    static void write_uncompressed_blocks(IO::SeekableReader auto &in_data, IO::BitWriter &out_writer, bool final)
    {
        auto data_length = in_data.length().value();

        do
        {
            uint16_t len = MIN(data_length, UINT16_MAX);

            write_uncompressed_block(in_data, len, out_writer, final && (len == data_length));
            data_length -= len;
        } while (data_length != 0);
    }

    static void write_block_header(IO::BitWriter &out_writer, BlockType block_type, bool final);
    static void write_uncompressed_block(IO::Reader &in_data, uint16_t block_len, IO::BitWriter &out_writer, bool final);

public:
    Deflate(unsigned int compression_level);

    Result perform(IO::SeekableReader auto &uncompressed, IO::Writer &compressed)
    {
        // If the data amount is too small it's not worth compressing it.
        // Depends on the compression level
        if (uncompressed.length().value() < _min_size_to_compress)
            [[unlikely]]
            {
                return compress_none(uncompressed, compressed);
            }

        switch (_compression_level)
        {
        // TODO: set the compression method according to the level
        default:
            return compress_none(uncompressed, compressed);
        }
    }
};

} // namespace Compression