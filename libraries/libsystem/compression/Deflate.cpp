#include <libsystem/Assert.h>
#include <libsystem/compression/Common.h>
#include <libsystem/compression/Deflate.h>
#include <libsystem/compression/Huffman.h>
#include <libsystem/io/Stream.h>

Deflate::Deflate(unsigned int compression_level) : _compression_level(compression_level)
{
    /*
	 * The higher the compression level, the more we should bother trying to
	 * compress very small inputs.
	 */
    _min_size_to_compress = 56 - (_compression_level * 4);

    switch (compression_level)
    {
    default:
        _compression_impl = compress_none;
    }
}

void Deflate::write_uncompressed_block(uint8_t *data, uint16_t data_len, BitWriter &out_stream, bool final)
{
    __unused(data);
    __unused(data_len);
    __unused(out_stream);
    __unused(final);
}

void Deflate::write_uncompressed_blocks(const Vector<uint8_t> &in_data, BitWriter &out_stream, bool final)
{
    uint8_t *data = in_data.raw_storage();
    auto data_length = in_data.count();

    do
    {
        uint16_t len = MIN(data_length, UINT16_MAX);

        write_uncompressed_block(data, len, out_stream, final && len == data_length);
        data += len;
        data_length -= len;
    } while (data_length != 0);
}

void Deflate::compress_none(const Vector<uint8_t> &uncompressed, Vector<uint8_t> &compressed)
{
    BitWriter bit_writer(compressed);
    write_uncompressed_blocks(uncompressed, bit_writer, true);
}