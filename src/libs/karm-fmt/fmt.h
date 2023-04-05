#pragma once

#include <karm-base/array.h>
#include <karm-base/res.h>
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
    usize base = 10;
    usize width = 0;
    bool fillZero = false;

    void parse(Text::Scan &scan) {
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

    Res<usize> formatUnsigned(Io::_TextWriter &writer, u64 value) {
        auto digit = [](usize v) {
            if (v < 10) {
                return '0' + v;
            } else {
                return 'a' + (v - 10);
            }
        };
        usize i = 0;
        Array<char, 128> buf;
#
        do {
            buf[i++] = digit(value % base);
            value /= base;
        } while (value != 0 && i < buf.len());

        reverse(mutSub(buf, 0, i));

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

        return writer.writeStr({buf.buf(), i});
    }

    Res<usize> formatSigned(Io::_TextWriter &writer, i64 value) {
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
    Res<usize> format(Io::_TextWriter &writer, T const value) {
        if (isChar) {
            return writer.writeRune(value);
        }
        return formatUnsigned(writer, value);
    }
};

template <typename T>
struct SignedFormatter : public NumberFormater {
    Res<usize> format(Io::_TextWriter &writer, T const value) {
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
struct Formatter<f64> {
    Res<usize> format(Io::_TextWriter &writer, f64 const value) {
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

template <>
struct Formatter<Str> {
    Res<usize> format(Io::_TextWriter &writer, Str text) {
        return writer.writeStr(text);
    }
};

template <>
struct Formatter<String> {
    Res<usize> format(Io::_TextWriter &writer, String text) {
        return writer.writeStr(text);
    }
};

template <>
struct Formatter<char const *> : public Formatter<Str> {};

struct _Args {
    virtual ~_Args() = default;
    virtual usize len() = 0;
    virtual Res<usize> format(Text::Scan &scan, Io::_TextWriter &writer, usize index) = 0;
};

template <typename... Ts>
struct Args : public _Args {
    Tuple<Ts...> _tuple{};

    Args(Ts &&...ts) : _tuple(std::forward<Ts>(ts)...) {}

    usize len() override { return _tuple.len(); }

    Res<usize> format(Text::Scan &scan, Io::_TextWriter &writer, usize index) override {
        Res<usize> result = Error("format index out of range");
        usize i = 0;
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

inline Res<usize> _format(Io::_TextWriter &writer, Str format, _Args &args) {
    Text::Scan scan{format};
    usize written = 0;
    usize index = 0;

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
            written += try$(writer.writeStr(Embed::LINE_ENDING));
        } else {
            written += try$(writer.writeRune(c));
        }
    }

    return Ok(written);
}

template <typename... Ts>
inline Res<usize> format(Io::_TextWriter &writer, Str format, Ts &&...ts) {
    Args<Ts...> args{std::forward<Ts>(ts)...};
    return _format(writer, format, args);
}

template <typename... Ts>
inline Res<String> format(Str format, Ts &&...ts) {
    Io::StringWriter writer{};
    Args<Ts...> args{std::forward<Ts>(ts)...};
    try$(_format(writer, format, args));
    return Ok(writer.take());
}

template <typename T>
inline Res<String> toStr(Str format, T const &t) {
    Io::StringWriter writer{};
    Formatter<T> formatter;
    if constexpr (requires(Text::Scan & scan) {
                      formatter.parse(scan);
                  }) {
        Text::Scan scan{format};
        formatter.parse(scan);
    }
    try$(formatter.format(writer, t));
    return Ok(writer.take());
}

template <typename T>
inline Res<String> toStr(T const &t) {
    Io::StringWriter writer{};
    Formatter<T> formatter;
    try$(formatter.format(writer, t));
    return Ok(writer.take());
}

} // namespace Karm::Fmt
