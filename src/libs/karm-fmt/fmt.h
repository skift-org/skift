#pragma once

#include <karm-base/array.h>
#include <karm-base/result.h>
#include <karm-base/rune.h>
#include <karm-base/string.h>
#include <karm-base/tuple.h>
#include <karm-io/funcs.h>
#include <karm-io/traits.h>
#include <karm-meta/signess.h>
#include <karm-meta/traits.h>
#include <karm-text/scan.h>

namespace Karm::Fmt {

struct NumberFormater {
    bool isChar = false;
    int base = 10;
    int width = 0;
    bool fillZero = false;

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

        case 'c':
            isChar = true;
            break;

        default:
            break;
        }
    }

    Result<size_t> formatUnsigned(Io::_TextWriter &writer, uint64_t value) {
        auto digit = [](size_t v) {
            if (v < 10) {
                return '0' + v;
            } else {
                return 'a' + (v - 10);
            }
        };
        size_t i = 0;
        Array<char, 128> buf;
#
        do {
            buf[i++] = digit(value % base);
            value /= base;
        } while (value != 0);

        reverse(mutSub(buf, 0, i));

        if (width > 0) {
            size_t n = width - i;
            if (fillZero) {
                for (size_t j = 0; j < n; j++) {
                    buf[i++] = '0';
                }
            } else {
                for (size_t j = 0; j < n; j++) {
                    buf[i++] = ' ';
                }
            }
        }

        return writer.writeStr({buf.buf(), i});
    }

    Result<size_t> formatSigned(Io::_TextWriter &writer, int64_t value) {
        size_t written = 0;
        if (value < 0) {
            written += try$(writer.writeRune('-'));
            value = -value;
        }

        written += try$(formatUnsigned(writer, value));
        return written;
    }
};

template <typename T>
struct UnsignedFormatter : public NumberFormater {
    Result<size_t> format(Io::_TextWriter &writer, T const value) {
        if (isChar) {
            return writer.writeRune(value);
        }
        return formatUnsigned(writer, value);
    }
};

template <typename T>
struct SignedFormatter : public NumberFormater {
    Result<size_t> format(Io::_TextWriter &writer, T const value) {
        if (isChar) {
            return writer.writeRune(value);
        }

        return formatSigned(writer, value);
    }
};

template <typename T>
struct Formatter;

template <Meta::UnsignedIntegral T>
struct Formatter<T> : public UnsignedFormatter<T> {};

template <Meta::SignedIntegral T>
struct Formatter<T> : public SignedFormatter<T> {};

template <>
struct Formatter<double> {
    Result<size_t> format(Io::_TextWriter &writer, double const value) {
        NumberFormater formater;
        size_t written = 0;
        int ipart = (int)value;
        written += try$(formater.formatSigned(writer, ipart));
        double fpart = value - (double)ipart;
        if (fpart != 0.0) {
            written += try$(writer.writeRune('.'));
            formater.width = 6;
            formater.fillZero = true;
            fpart *= 1000000;
            written += try$(formater.formatUnsigned(writer, (uint64_t)fpart));
        }
        return written;
    }
};

template <>
struct Formatter<Str> {
    Result<size_t> format(Io::_TextWriter &writer, Str text) {
        return writer.writeStr(text);
    }
};

template <>
struct Formatter<String> {
    Result<size_t> format(Io::_TextWriter &writer, String text) {
        return writer.writeStr(text);
    }
};

template <>
struct Formatter<char const *> : public Formatter<Str> {};

struct _Args {
    virtual ~_Args() = default;
    virtual size_t len() = 0;
    virtual Result<size_t> format(Text::Scan &scan, Io::_TextWriter &writer, size_t index) = 0;
};

template <typename... Ts>
struct Args : public _Args {
    Tuple<Ts...> _tuple{};

    Args(Ts &&...ts) : _tuple(std::forward<Ts>(ts)...) {}

    size_t len() override { return _tuple.len(); }

    Result<size_t> format(Text::Scan &scan, Io::_TextWriter &writer, size_t index) override {
        Result<size_t> result = 0;
        size_t i = 0;
        _tuple.visit([&](auto const &t) {
            if (index == i) {
                using U = Meta::RemoveConstVolatileRef<decltype(t)>;
                Formatter<U> formatter;
                if constexpr (requires() {
                                  formatter.parse(scan);
                              }) {
                    formatter.parse(scan);
                }
                result = formatter.format(writer, t);
            }
            i++;
        });
        return result;
    }
};

inline Result<size_t> _format(Io::_TextWriter &writer, Str format, _Args &args) {
    Text::Scan scan{format};
    size_t written = 0;
    size_t index = 0;

    while (not scan.ended()) {
        Rune c = scan.next();

        if (c == '{') {
            scan.begin();
            scan.skip(':');
            while (scan.curr() != '}') {
                scan.next();
            }
            scan.next();
            Text::Scan inner{scan.end()};
            written += try$(args.format(inner, writer, index));
            index++;
        } else if (c == '\n') {
            // normalize newlines
            written += try$(writer.writeStr(EMBED_SYS_LINE_ENDING));
        } else {
            written += try$(writer.writeRune(c));
        }
    }

    return written;
}

template <typename... Ts>
inline Result<size_t> format(Io::_TextWriter &writer, Str format, Ts &&...ts) {
    Args<Ts...> args{std::forward<Ts>(ts)...};
    return _format(writer, format, args);
}

template <typename... Ts>
inline String format(Str format, Ts &&...ts) {
    Io::StringWriter writer{};
    Args<Ts...> args{std::forward<Ts>(ts)...};
    _format(writer, format, args).unwrap();
    return writer.finalize();
}

} // namespace Karm::Fmt
