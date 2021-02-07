#include <assert.h>
#include <libsystem/compression/Common.h>
#include <libsystem/compression/Deflate.h>
#include <libsystem/compression/Huffman.h>
#include <libsystem/io/Reader.h>
#include <libsystem/io/Writer.h>

Deflate::Deflate(unsigned int compression_level) : _compression_level(compression_level)
{
    /*
	 * The higher the compression level, the more we should bother trying to
	 * compress very small inputs.
	 */
    _min_size_to_compress = 56 - (_compression_level * 4);

    switch (compression_level)
    {
    // TODO: set the compression method according to the level
    default:
        _compression_method = compress_none;
    }
}

void Deflate::write_block_header(BitWriter &out_writer, BlockType block_type, bool final)
{
    out_writer.put_bits(final ? 1 : 0, 1);
    out_writer.put_bits(block_type, 2);
}

void Deflate::write_uncompressed_block(Reader &in_data, uint16_t data_len, BitWriter &out_writer, bool final)
{
    Vector<uint8_t> data(data_len);
    assert(in_data.read(data.raw_storage(), data_len) == data_len);

    // Write the data
    write_block_header(out_writer, BlockType::BT_UNCOMPRESSED, final);
    out_writer.align();
    out_writer.put_uint16(data_len);
    out_writer.put_uint16(~data_len);
    out_writer.put_data(data.raw_storage(), data_len);
}

void Deflate::write_uncompressed_blocks(Reader &in_data, BitWriter &out_writer, bool final)
{
    auto data_length = in_data.length();

    do
    {
        uint16_t len = MIN(data_length, UINT16_MAX);

        write_uncompressed_block(in_data, len, out_writer, final && (len == data_length));
        data_length -= len;
    } while (data_length != 0);
}

Result Deflate::compress_none(Reader &uncompressed, Writer &compressed)
{
    BitWriter bit_writer(compressed);
    write_uncompressed_blocks(uncompressed, bit_writer, true);

    return Result::SUCCESS;
}

Result Deflate::perform(Reader &uncompressed, Writer &compressed)
{
    // If the data amount is too small it's not worth compressing it.
    // Depends on the compression level
    if (uncompressed.length() < _min_size_to_compress)
        [[unlikely]]
        {
            return compress_none(uncompressed, compressed);
        }

    return _compression_method(uncompressed, compressed);
}