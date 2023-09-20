#pragma once

#include <karm-meta/signess.h>

#include "base.h"

namespace Karm::Fmt {

struct NumberFormater {
    bool isChar = false;
    usize base = 10;
    usize width = 0;
    bool fillZero = false;

    void parse(Io::SScan &scan) {
        if (scan.ended()) {
            return;
        }

        if (scan.peek() == '0') {
            scan.next();
            fillZero = true;
        }

        if (scan.ended()) {
            return;
        }

        width = tryOr(scan.nextInt(), 0);

        Rune c = scan.next();

        switch (c) {
        case 'b':
            base = 2;
            break;

        case 'o':
            base = 8;
            break;

        case 'd':
            base = 10;
            break;

        case 'x':
            base = 16;
            break;

        case 'p':
            base = 16;
            fillZero = true;
            width = sizeof(usize) * 2;
            break;

        case 'c':
            isChar = true;
            break;

        default:
            break;
        }
    }

    Res<usize> formatUnsigned(Io::TextWriter &writer, u64 value) {
        auto digit = [](usize v) {
            if (v < 10) {
                return '0' + v;
            } else {
                return 'a' + (v - 10);
            }
        };
        usize i = 0;
        Array<char, 128> buf;

        do {
            buf[i++] = digit(value % base);
            value /= base;
        } while (value != 0 and i < buf.len());

        if (width > 0) {
            usize n = width - i;
            if (fillZero) {
                for (usize j = 0; j < n; j++) {
                    buf[i++] = '0';
                }
            } else {
                for (usize j = 0; j < n; j++) {
                    buf[i++] = ' ';
                }
            }
        }

        reverse(mutSub(buf, 0, i));
        return writer.writeStr({buf.buf(), i});
    }

    Res<usize> formatSigned(Io::TextWriter &writer, i64 value) {
        usize written = 0;
        if (value < 0) {
            written += try$(writer.writeRune('-'));
            value = -value;
        }

        written += try$(formatUnsigned(writer, value));
        return Ok(written);
    }
};

template <typename T>
struct UnsignedFormatter : public NumberFormater {
    Res<usize> format(Io::TextWriter &writer, T const value) {
        if (isChar) {
            return writer.writeRune(value);
        }
        return formatUnsigned(writer, value);
    }
};

template <typename T>
struct SignedFormatter : public NumberFormater {
    Res<usize> format(Io::TextWriter &writer, T const value) {
        if (isChar) {
            return writer.writeRune(value);
        }

        return formatSigned(writer, value);
    }
};

template <Meta::UnsignedIntegral T>
struct Formatter<T> : public UnsignedFormatter<T> {};

template <Meta::SignedIntegral T>
struct Formatter<T> : public SignedFormatter<T> {};

template <>
struct Formatter<f64> {
    Res<usize> format(Io::TextWriter &writer, f64 const value) {
        NumberFormater formater;
        usize written = 0;
        isize ipart = (isize)value;
        written += try$(formater.formatSigned(writer, ipart));
        f64 fpart = value - (f64)ipart;
        if (fpart != 0.0) {
            written += try$(writer.writeRune('.'));
            formater.width = 6;
            formater.fillZero = true;
            fpart *= 1000000;
            written += try$(formater.formatUnsigned(writer, (u64)fpart));
        }
        return Ok(written);
    }
};

} // namespace Karm::Fmt
