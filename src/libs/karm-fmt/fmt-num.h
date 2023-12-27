#pragma once

#include <karm-base/endian.h>
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

    Res<usize> formatUnsigned(Io::TextWriter &writer, usize val) {
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
            buf[i++] = digit(val % base);
            val /= base;
        } while (val != 0 and i < buf.len());

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

    Res<usize> formatSigned(Io::TextWriter &writer, isize val) {
        usize written = 0;
        if (val < 0) {
            written += try$(writer.writeRune('-'));
            val = -val;
        }
        written += try$(formatUnsigned(writer, val));
        return Ok(written);
    }
};

template <typename T>
struct UnsignedFormatter : public NumberFormater {
    Res<usize> format(Io::TextWriter &writer, T const &val) {
        if (isChar)
            return writer.writeRune(val);
        return formatUnsigned(writer, val);
    }
};

template <typename T>
struct SignedFormatter : public NumberFormater {
    Res<usize> format(Io::TextWriter &writer, T const &val) {
        if (isChar) {
            return writer.writeRune(val);
        }

        return formatSigned(writer, val);
    }
};

template <Meta::UnsignedIntegral T>
struct Formatter<T> : public UnsignedFormatter<T> {};

template <Meta::SignedIntegral T>
struct Formatter<T> : public SignedFormatter<T> {};

template <>
struct Formatter<f64> {
    Res<usize> format(Io::TextWriter &writer, f64 const &val) {
        NumberFormater formater;
        usize written = 0;
        isize ipart = (isize)val;
        written += try$(formater.formatSigned(writer, ipart));
        f64 fpart = val - (f64)ipart;
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

template <typename T>
struct Formatter<Be<T>> : public Formatter<T> {
    Res<usize> format(Io::TextWriter &writer, Be<T> const &val) {
        return Formatter<T>::format(writer, val.value());
    }
};

template <typename T>
struct Formatter<Le<T>> : public Formatter<T> {
    Res<usize> format(Io::TextWriter &writer, Le<T> const &val) {
        return Formatter<T>::format(writer, val.value());
    }
};

} // namespace Karm::Fmt
