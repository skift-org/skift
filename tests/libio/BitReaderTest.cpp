#include <libio/BitReader.h>
#include <libtest/AssertEqual.h>
#include <libtest/AssertFalse.h>
#include <libtest/AssertNotEqual.h>
#include <libtest/AssertTrue.h>

#include "tests/Driver.h"

TEST(bitreader_peek)
{
    uint8_t data[] = {0b01101001};
    IO::MemoryReader mem_reader(data, sizeof(data));
    IO::BitReader bit_reader(mem_reader);

    assert_equal(bit_reader.peek_bits(4), 6);
    assert_equal(bit_reader.grab_bits(4), 6);

    assert_equal(bit_reader.peek_bits(4), 9);
    assert_equal(bit_reader.grab_bits(4), 9);
}