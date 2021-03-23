#include <libio/BitReader.h>
#include <libio/ReadCounter.h>
#include <libio/Repeat.h>
#include <libtest/Asserts.h>

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

#define assert_count_and_reset(__count)     \
    assert_equal(counter.count(), __count); \
    counter.reset();                        \
    reader.flush();

TEST(bitreader_should_only_fetch_what_it_need)
{
    IO::Repeat repeat{0xfc};
    IO::ReadCounter counter{repeat};
    IO::BitReader reader{counter};

    assert_equal(counter.count(), 0);

    // hint function
    reader.hint(0);
    assert_count_and_reset(0);

    reader.hint(5);
    assert_count_and_reset(1);

    reader.hint(8);
    assert_count_and_reset(1);

    reader.hint(16);
    assert_count_and_reset(2);

    // skip function
    reader.skip_bits(0);
    assert_count_and_reset(0);

    reader.skip_bits(5);
    assert_count_and_reset(1);

    reader.skip_bits(8);
    assert_count_and_reset(1);

    reader.skip_bits(16);
    assert_count_and_reset(2);

    // grab_aligned
    reader.grab_bits(5);
    reader.grab<uint16_t>();
    assert_count_and_reset(3);
}