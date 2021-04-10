#include <libio/Format.h>

#include "tests/Driver.h"

#define FMT(__format, __value, __expected) \
    Assert::equal(IO::format("'{" __format "}'", __value), "'" __expected "'")

TEST(format_int_formating)
{
    FMT("", 0, "0");

    FMT("", 1, "1");
    FMT("", 10, "10");
    FMT("", 100, "100");
    FMT("", 1234, "1234");

    FMT("", -1, "-1");
    FMT("", -10, "-10");
    FMT("", -100, "-100");
    FMT("", -1234, "-1234");
}

TEST(format_float_formating)
{
    FMT(".4", 0., "0.0000");

    FMT(".4", 1., "1.0000");
    FMT(".4", 10., "10.0000");
    FMT(".4", 100., "100.0000");
    FMT(".4", 1234., "1234.0000");
    FMT(".4", 1234.5678, "1234.5678");

    FMT(".4", -1., "-1.0000");
    FMT(".4", -10., "-10.0000");
    FMT(".4", -100., "-100.0000");
    FMT(".4", -1234., "-1234.0000");
    FMT(".4", -1234.5678, "-1234.5678");

    FMT(".4", (0.0 / 0.0), "nan");
    FMT(".4", (1.0 / 0.0), "inf");
    FMT(".4", (-1.0 / 0.0), "-inf");
}
