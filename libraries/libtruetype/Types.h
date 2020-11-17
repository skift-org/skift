#pragma once

#include <libutils/Endian.h>

namespace truetype
{

// 8-bit unsigned integer.
using tt_uint8_t = be_uint8_t;

// 8-bit signed integer.
using tt_int8_t = be_int8_t;

// 16-bit unsigned integer.
using tt_uint16_t = be_uint16_t;

// 16-bit signed integer.
using tt_int16_t = be_int16_t;

// 32-bit unsigned integer.
using tt_uint32_t = be_uint32_t;

// 32-bit signed integer.
using tt_int32_t = be_int32_t;

// 32-bit signed fixed-point number.
using tt_fixed_t = be_uint32_t;

// int16 that describes a quantity in font design units.
using tt_fword_t = be_int16_t;

// uint16 that describes a quantity in font design units.
using tt_ufword_t = be_uint16_t;

// 16-bit signed fixed number with the low 14 bits of fraction (2.14).
using tt_f2dot14_t = be_int16_t;

// Date and time represented in number of seconds since 12:00 midnight,
// January 1, 1904, UTC. The value is represented as a signed 64-bit integer.
using tt_longdatetime_t = be_int64_t;

// Short offset to a table, same as uint16, NULL offset = 0x0000
using tt_offset16_t = be_uint16_t;

// Long offset to a table, same as uint32, NULL offset = 0x00000000
using tt_offset32_t = be_uint32_t;

// Packed 32-bit value with major and minor version numbers. (See Table Version Numbers.)
using tt_ver16dot16_t = be_uint32_t;

} // namespace truetype
