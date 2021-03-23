#include <libcompression/Inflate.h>
#include <libtest/Asserts.h>

#include "tests/Driver.h"

TEST(inflate_empty)
{
    /* Empty buffer, fixed, 6 bits of padding in the second byte set to 1 */
    const uint8_t data[] = {
        0x03, 0xFC};

    IO::MemoryReader mem_reader(data, sizeof(data));
    IO::MemoryWriter mem_writer;

    Compression::Inflate inf;

    auto result = inf.perform(mem_reader, mem_writer);
    // The inflate should have been succesful
    assert_equal(result.result(), Result::SUCCESS);
    // The algorithm consumed all input bytes
    assert_equal(result.value(), sizeof(data));
    // Should have generated no output
    assert_equal(mem_writer.length().value(), 0);
}

TEST(inflate_huffman)
{
    /* 256 zero bytes compressed using Huffman only (no match or distance codes) */
    const uint8_t data[] = {
        0x05, 0xCA, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0xFF,
        0xD5, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x02};

    IO::MemoryReader mem_reader(data, sizeof(data));
    IO::MemoryWriter mem_writer;

    Compression::Inflate inf;

    auto result = inf.perform(mem_reader, mem_writer);
    // The inflate should have been succesful
    assert_equal(result.result(), Result::SUCCESS);
    // The algorithm consumed all input bytes
    assert_equal(result.value(), sizeof(data));
    // Should have generated 256 bytes
    assert_equal(mem_writer.length().value(), 256);
    // All bytes should be zero
    auto uncompressed = Slice(mem_writer.slice());
    for (size_t i = 0; i < uncompressed.size(); i++)
    {
        assert_equal(((uint8_t *)uncompressed.start())[i], 0);
    }
}

TEST(inflate_rle)
{
    /* 256 zero bytes compressed using RLE (only one distance code) */
    const uint8_t data[] = {
        0xE5, 0xC0, 0x81, 0x00, 0x00, 0x00, 0x00, 0x80, 0xA0, 0xFC,
        0xA9, 0x07, 0x39, 0x73, 0x01};

    IO::MemoryReader mem_reader(data, sizeof(data));
    IO::MemoryWriter mem_writer;

    Compression::Inflate inf;

    auto result = inf.perform(mem_reader, mem_writer);
    // The inflate should have been succesful
    assert_equal(result.result(), Result::SUCCESS);
    // The algorithm consumed all input bytes
    assert_equal(result.value(), sizeof(data));
    // Should have generated 256 bytes
    assert_equal(mem_writer.length().value(), 256);
    // All bytes should be zero
    auto uncompressed = Slice(mem_writer.slice());
    for (size_t i = 0; i < uncompressed.size(); i++)
    {
        assert_equal(((uint8_t *)uncompressed.start())[i], 0);
    }
}