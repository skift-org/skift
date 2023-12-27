#pragma once

#include <karm-io/funcs.h>
#include <karm-io/traits.h>

namespace Karm::Crypto {

namespace Base64 {

static constexpr Str TABLE = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

Res<usize> encode(Io::Reader &in, Io::TextWriter &out, Str table = TABLE) {
    usize written = 0;
    Array<Byte, 3> buf;

    while (true) {
        auto read = try$(in.read(buf));
        if (not read)
            break;

        u8 a = buf[0];
        u8 b = buf[1];
        u8 c = buf[2];

        u8 x = a >> 2;
        u8 y = ((a & 0b11) << 4) | (b >> 4);
        u8 z = ((b & 0b1111) << 2) | (c >> 6);
        u8 w = c & 0b111111;

        try$(out.writeRune(table[x]));
        try$(out.writeRune(table[y]));
        try$(out.writeRune(read > 1 ? table[z] : '='));
        try$(out.writeRune(read > 2 ? table[w] : '='));
        written += 4;
    }

    return Ok(written);
}

Res<usize> encode(Bytes bytes, Io::TextWriter &out, Str table = TABLE) {
    Io::BufReader reader(bytes);
    return encode(reader, out, table);
}

Res<usize> decode(Io::Reader &in, Io::Writer &out, Str table = TABLE) {
    usize written = 0;
    Array<Byte, 4> buf;

    while (true) {
        auto read = try$(in.read(buf));
        if (not read)
            break;

        u8 a = 0;
        u8 b = 0;
        u8 c = 0;
        u8 d = 0;

        for (usize i = 0; i < read; i++) {
            auto ch = buf[i];
            if (ch == '=')
                break;

            auto maybeIdx = indexOf(table, ch);
            if (not maybeIdx)
                return Error::invalidInput("invalid  character");

            auto idx = maybeIdx.unwrap();

            switch (i) {
            case 0:
                a = idx;
                break;
            case 1:
                b = idx;
                break;
            case 2:
                c = idx;
                break;
            case 3:
                d = idx;
                break;
            }
        }

        u8 x = (a << 2) | (b >> 4);
        u8 y = ((b & 0b1111) << 4) | (c >> 2);
        u8 z = ((c & 0b11) << 6) | d;

        try$(Io::putByte(out, x));
        if (read > 2)
            try$(Io::putByte(out, y));
        if (read > 3)
            try$(Io::putByte(out, z));
        written += read - 1;
    }

    return Ok(written);
}

Res<usize> decode(Str str, Io::Writer &out, Str table = TABLE) {
    Io::BufReader reader(bytes(str));
    return decode(reader, out, table);
}

} // namespace Base64

} // namespace Karm::Crypto
