#include <libcompression/Common.h>
#include <libcompression/Deflate.h>
#include <libcompression/Huffman.h>
#include <libtest/Asserts.h>

namespace Compression
{

Deflate::Deflate(unsigned int compression_level) : _compression_level(compression_level)
{
    /*
	 * The higher the compression level, the more we should bother trying to
	 * compress very small inputs.
	 */
    _min_size_to_compress = 56 - (_compression_level * 4);
}

void Deflate::write_block_header(IO::BitWriter &out_writer, BlockType block_type, bool final)
{
    out_writer.put_bits(final ? 1 : 0, 1);
    out_writer.put_bits(block_type, 2);
}

void Deflate::write_uncompressed_block(IO::Reader &in_data, uint16_t data_len, IO::BitWriter &out_writer, bool final)
{
    Vector<uint8_t> data(data_len);
    assert_equal(in_data.read(data.raw_storage(), data_len).value(), data_len);

    // Write the data
    write_block_header(out_writer, BlockType::BT_UNCOMPRESSED, final);
    out_writer.align();
    out_writer.put_uint16(data_len);
    out_writer.put_uint16(~data_len);
    out_writer.put_data(data.raw_storage(), data_len);
}

} // namespace Compression