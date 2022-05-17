#pragma once

#include <karm-base/array.h>
#include <karm-base/result.h>
#include <karm-base/rune.h>
#include <karm-base/string.h>
#include <karm-base/tuple.h>
#include <karm-io/funcs.h>
#include <karm-io/traits.h>
#include <karm-text/emit.h>
#include <karm-text/scan.h>

namespace Karm::Fmt {

struct NumberFormater {
    int base = 10;

    void parse(Text::Scan &scan) {
        if (scan.ended()) {
            return;
        }

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

        default:
            break;
        }
    }

    Result<size_t> format_unsigned(Io::Writer &writer, uint64_t value) {
        size_t i = 0;
        Array<uint8_t, 65> buf;

        do {
            buf[i++] = value % base + '0';
            value /= base;
        } while (value != 0);

        return writer.write(buf.buf(), buf.len());
    }
};

template <typename T>
struct UnsignedFormatter : public NumberFormater {
    Result<size_t> format(Io::Writer &writer, T const value) {
        return format_unsigned(writer, value);
    }
};

template <typename T>
struct SignedFormatter : public NumberFormater {
    Result<size_t> format(Io::Writer &writer, T const value) {
        size_t written = 0;
        if (value < 0) {
            written += try$(Io::putc(writer, '-'));
            value = -value;
        }

        return written + format_unsigned(writer, value);
    }
};

template <typename T>
struct Formatter;

template <>
struct Formatter<uint8_t> : public UnsignedFormatter<uint8_t> {};

template <>
struct Formatter<uint16_t> : public UnsignedFormatter<uint16_t> {};

template <>
struct Formatter<uint32_t> : public UnsignedFormatter<uint32_t> {};

template <>
struct Formatter<uint64_t> : public UnsignedFormatter<uint64_t> {};

template <>
struct Formatter<int8_t> : public SignedFormatter<int8_t> {};

template <>
struct Formatter<int16_t> : public SignedFormatter<int16_t> {};

template <>
struct Formatter<int32_t> : public SignedFormatter<int32_t> {};

template <>
struct Formatter<int64_t> : public SignedFormatter<int64_t> {};

template <>
struct Formatter<Str> {
    void parse(Text::Scan &) {
    }

    Result<size_t> format(Io::Writer &writer, Str const &text) {
        return writer.write(text.buf(), text.len());
    }
};

struct _Args {
    virtual size_t len() = 0;
    virtual Result<size_t> format(Text::Scan &scan, Io::Writer &writer, size_t index) = 0;
};

template <typename... Ts>
struct Args : public _Args {
    Tuple<Ts...> _tuple;

    Args(Ts &&...ts) : _tuple(std::forward<Ts>(ts)...) {}

    size_t len() override { return _tuple.len(); }

    Result<size_t> format(Text::Scan &scan, Io::Writer &writer, size_t index) override {
        Result<size_t> result = 0;
        _tuple.visit([&](auto const &t) {
            if (index == 0) {
                using U = Meta::RemoveConstVolatileRef<decltype(t)>;
                Formatter<U> formatter;
                formatter.parse(scan);
                result = formatter.format(writer, t);
            } else {
                index--;
            }
        });
        return result;
    }
};

Result<size_t> _format(Io::Writer &writer, Str format, _Args &args) {
    Text::Scan scan{format};
    size_t written = 0;
    size_t index = 0;

    while (!scan.ended()) {
        Rune c = scan.next();

        if (c == '{') {
            scan.begin();
            scan.skip(':');
            while (scan.peek() != '}') {
                scan.next();
            }
            Text::Scan inner{scan.end()};
            written += try$(args.format(inner, writer, index));
            index++;
        } else {
            written += try$(Io::putr(writer, c));
        }
    }

    return written;
}

template <typename... Ts>
Result<size_t> format(Io::Writer &writer, Str format, Ts &&...ts) {
    Args<Ts...> args{std::forward<Ts>(ts)...};
    return _format(writer, format, args);
}

} // namespace Karm::Fmt
