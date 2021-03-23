#include <libio/BitReader.h>
#include <libtest/AssertEqual.h>
#include <libtest/AssertFalse.h>
#include <libtest/AssertNotEqual.h>
#include <libtest/AssertTrue.h>

#include "tests/Driver.h"

TEST(bitreader_peek_bits)
{
    uint8_t data[] = {0b01001000, 0b11000011, 0b01011010, 0b11111111, 0b00000000};
    IO::MemoryReader mem_reader(data, sizeof(data));
    IO::BitReader bit_reader(mem_reader);

    // Grab 1000
    assert_equal(bit_reader.peek_bits(4), 8);
    assert_equal(bit_reader.grab_bits(4), 8);

    // Grab 0100
    assert_equal(bit_reader.peek_bits(4), 4);
    assert_equal(bit_reader.grab_bits(4), 4);

    // Grab 11
    assert_equal(bit_reader.peek_bits(2), 3);
    assert_equal(bit_reader.grab_bits(2), 3);

    // Grab 10000
    assert_equal(bit_reader.peek_bits(5), 16);
    assert_equal(bit_reader.grab_bits(5), 16);

    // Grab 101
    assert_equal(bit_reader.peek_bits(3), 5);
    assert_equal(bit_reader.grab_bits(3), 5);
}

TEST(bitreader_peek_bits_reverse)
{
    uint8_t data[] = {0b01001000, 0b11000011, 0b01011010, 0b11111111, 0b00000000};
    IO::MemoryReader mem_reader(data, sizeof(data));
    IO::BitReader bit_reader(mem_reader);

    // Grab 0001
    assert_equal(bit_reader.peek_bits_reverse(4), 1);
    assert_equal(bit_reader.grab_bits_reverse(4), 1);

    // Grab 0010
    assert_equal(bit_reader.peek_bits_reverse(4), 2);
    assert_equal(bit_reader.grab_bits_reverse(4), 2);

    // Grab 11
    assert_equal(bit_reader.peek_bits_reverse(2), 3);
    assert_equal(bit_reader.grab_bits_reverse(2), 3);

    // Grab 00001
    assert_equal(bit_reader.peek_bits_reverse(5), 1);
    assert_equal(bit_reader.grab_bits_reverse(5), 1);

    // Grab 101
    assert_equal(bit_reader.peek_bits_reverse(3), 5);
    assert_equal(bit_reader.grab_bits_reverse(3), 5);
}