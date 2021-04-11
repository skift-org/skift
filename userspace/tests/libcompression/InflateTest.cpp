#include <libcompression/Inflate.h>

#include "tests/Driver.h"

// Test from https://github.com/jibsen/tinf
// Copyright (c) 2003-2019 Joergen Ibsen

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
    Assert::equal(result.result(), Result::SUCCESS);
    // The algorithm consumed all input bytes
    Assert::equal(result.unwrap(), sizeof(data));
    // Should have generated no output
    Assert::equal(mem_writer.length().unwrap(), 0);
}

TEST(inflate_empty_no_literals)
{
    /* Empty buffer, dynamic with 256 as only literal/length code
	 *
	 * You could argue that since the RFC only has an exception allowing
	 * one symbol for the distance tree, the literal/length tree should
	 * be complete. However gzip allows this.
	 *
	 * See also: https://github.com/madler/zlib/issues/75
	 */
    static const unsigned char data[] = {
        0x05, 0xCA, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0xFF,
        0x6B, 0x01, 0x00};

    IO::MemoryReader mem_reader(data, sizeof(data));
    IO::MemoryWriter mem_writer;
    Compression::Inflate inf;
    auto result = inf.perform(mem_reader, mem_writer);

    Assert::equal(result.result(), Result::SUCCESS);
    Assert::equal(mem_writer.length().unwrap(), 0);
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
    Assert::equal(result.result(), Result::SUCCESS);
    // The algorithm consumed all input bytes
    Assert::equal(result.unwrap(), sizeof(data));
    // Should have generated 256 bytes
    Assert::equal(mem_writer.length().unwrap(), 256);
    // All bytes should be zero
    auto uncompressed = Slice(mem_writer.slice());
    for (size_t i = 0; i < uncompressed.size(); i++)
    {
        Assert::equal(((uint8_t *)uncompressed.start())[i], 0);
    }
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
    Assert::equal(result.result(), Result::SUCCESS);
    // The algorithm consumed all input bytes
    Assert::equal(result.unwrap(), sizeof(data));
    // Should have generated 256 bytes
    Assert::equal(mem_writer.length().unwrap(), 256);
    // All bytes should be zero
    auto uncompressed = Slice(mem_writer.slice());
    for (size_t i = 0; i < uncompressed.size(); i++)
    {
        Assert::equal(((uint8_t *)uncompressed.start())[i], 0);
    }
}

TEST(inflate_max_matchlen)
{
    /* 259 zero bytes compressed using literal/length code 285 (len 258) */
    static const unsigned char data[] = {
        0xED, 0xCC, 0x81, 0x00, 0x00, 0x00, 0x00, 0x80, 0xA0, 0xFC,
        0xA9, 0x17, 0xB9, 0x00, 0x2C};

    IO::MemoryReader mem_reader(data, sizeof(data));
    IO::MemoryWriter mem_writer;
    Compression::Inflate inf;
    auto result = inf.perform(mem_reader, mem_writer);

    // The inflate should have been succesful
    Assert::equal(result.result(), Result::SUCCESS);

    // The algorithm consumed all input bytes
    Assert::equal(result.unwrap(), sizeof(data));

    // All bytes should be zero
    auto uncompressed = Slice(mem_writer.slice());
    for (size_t i = 0; i < uncompressed.size(); i++)
    {
        Assert::equal(((uint8_t *)uncompressed.start())[i], 0);
    }
}

TEST(inflate_max_matchlen_alt)
{
    /* 259 zero bytes compressed using literal/length code 284 + 31 (len 258)
	 *
	 * Technically, this is outside the range specified in the RFC, but
	 * gzip allows it.
	 *
	 * See also: https://github.com/madler/zlib/issues/75
	 */
    static const unsigned char data[] = {
        0xE5, 0xCC, 0x81, 0x00, 0x00, 0x00, 0x00, 0x80, 0xA0, 0xFC,
        0xA9, 0x07, 0xB9, 0x00, 0xFC, 0x05};

    IO::MemoryReader mem_reader(data, sizeof(data));
    IO::MemoryWriter mem_writer;
    Compression::Inflate inf;
    auto result = inf.perform(mem_reader, mem_writer);

    // The inflate should have been succesful
    Assert::equal(result.result(), Result::SUCCESS);

    // The algorithm consumed all input bytes
    Assert::equal(result.unwrap(), sizeof(data));

    // All bytes should be zero
    auto uncompressed = Slice(mem_writer.slice());
    for (size_t i = 0; i < uncompressed.size(); i++)
    {
        Assert::equal(((uint8_t *)uncompressed.start())[i], 0);
    }
}

TEST(inflate_max_matchdist)
{
    /* A match of length 3 with a distance of 32768 */
    static const unsigned char data[] = {
        0xED, 0xDD, 0x01, 0x01, 0x00, 0x00, 0x08, 0x02, 0x20, 0xED,
        0xFF, 0xE8, 0xFA, 0x11, 0x1C, 0x61, 0x9A, 0xF7, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0,
        0xFE, 0xFF, 0x05};

    IO::MemoryReader mem_reader(data, sizeof(data));
    IO::MemoryWriter mem_writer;
    Compression::Inflate inf;
    auto result = inf.perform(mem_reader, mem_writer);

    // The inflate should have been succesful
    Assert::equal(result.result(), Result::SUCCESS);

    // The algorithm consumed all input bytes
    Assert::equal(result.unwrap(), sizeof(data));

    Assert::equal(mem_writer.length().unwrap(), 32771);

    auto uncompressed = Slice(mem_writer.slice());
    const uint8_t *out = (const uint8_t *)uncompressed.start();

    Assert::equal(out[0], 2);
    Assert::equal(out[1], 1);
    Assert::equal(out[2], 0);

    for (size_t i = 3; i < uncompressed.size() - 3; i++)
    {
        Assert::equal(((uint8_t *)uncompressed.start())[i], 0);
    }

    Assert::equal(out[uncompressed.size() - 3], 2);
    Assert::equal(out[uncompressed.size() - 2], 1);
    Assert::equal(out[uncompressed.size() - 1], 0);
}
