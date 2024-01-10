#pragma once

#include <karm-base/endian.h>
#include <karm-base/rc.h>
#include <karm-base/reflect.h>
#include <karm-base/time.h>
#include <karm-base/tuple.h>
#include <karm-io/impls.h>
#include <karm-io/sscan.h>
#include <karm-io/traits.h>
#include <karm-meta/signess.h>

namespace Karm::Io {

template <typename T>
struct Formatter;

struct _Args {
    virtual ~_Args() = default;
    virtual usize len() = 0;
    virtual Res<usize> format(Io::SScan &scan, Io::TextWriter &writer, usize index) = 0;
};

template <typename... Ts>
struct Args : public _Args {
    Tuple<Ts...> _tuple{};

    Args(Ts &&...ts) : _tuple(std::forward<Ts>(ts)...) {}

    usize len() override {
        return _tuple.len();
    }

    Res<usize> format(Io::SScan &scan, Io::TextWriter &writer, usize index) override {
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

            return true;
        });
        return result;
    }
};

inline Res<usize> _format(Io::TextWriter &writer, Str format, _Args &args) {
    Io::SScan scan{format};
    usize written = 0;
    usize index = 0;

    while (not scan.ended()) {
        Rune c = scan.next();

        if (c == '{') {
            scan.skip(':');
            scan.begin();
            while (scan.curr() != '}') {
                scan.next();
            }
            scan.next();
            Io::SScan inner{scan.end()};
            written += try$(args.format(inner, writer, index));
            index++;
        } else if (c == '\n') {
            // normalize newlines
            written += try$(writer.writeStr(Sys::LINE_ENDING));
        } else {
            written += try$(writer.writeRune(c));
        }
    }

    return Ok(written);
}

template <typename... Ts>
inline Res<usize> format(Io::TextWriter &writer, Str format, Ts &&...ts) {
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
inline Res<String> toStr(T const &t, Str format = "") {
    Io::StringWriter writer{};
    Formatter<T> formatter;
    if constexpr (requires(Io::SScan &scan) {
                      formatter.parse(scan);
                  }) {
        Io::SScan scan{format};
        formatter.parse(scan);
    }
    try$(formatter.format(writer, t));
    return Ok(writer.take());
}

/* --- Align Formatting ----------------------------------------------------- */

enum struct Align {
    LEFT,
    RIGHT,
    CENTER,
};

template <typename T>
struct Aligned {
    T _inner;
    Align _align;
    usize _width;
};

inline auto aligned(auto inner, Align align, usize width) {
    return Aligned<decltype(inner)>{inner, align, width};
}

template <typename T>
struct Formatter<Aligned<T>> {
    Formatter<T> _innerFmt{};

    void parse(Io::SScan &scan) {
        if constexpr (requires() {
                          _innerFmt.parse(scan);
                      }) {
            _innerFmt.parse(scan);
        }
    }

    Res<usize> format(Io::TextWriter &writer, Aligned<T> val) {
        Io::StringWriter buf;
        try$(_innerFmt.format(buf, val._inner));
        usize width = buf.len();
        usize result = 0;

        if (width < val._width) {
            usize pad = val._width - width;
            switch (val._align) {
            case Align::LEFT:
                result += try$(writer.writeStr(buf.str()));
                for (usize i = 0; i < pad; i++)
                    result += try$(writer.writeRune(' '));
                break;
            case Align::RIGHT:
                for (usize i = 0; i < pad; i++)
                    result += try$(writer.writeRune(' '));
                result += try$(writer.writeStr(buf.str()));
                break;
            case Align::CENTER:
                for (usize i = 0; i < pad / 2; i++)
                    result += try$(writer.writeRune(' '));
                result += try$(writer.writeStr(buf.str()));
                for (usize i = 0; i < pad / 2; i++)
                    result += try$(writer.writeRune(' '));
                break;
            }
        } else {
            result += try$(writer.writeStr(buf.str()));
        }

        return Ok(result);
    }
};

/* --- Case Formatting ------------------------------------------------------ */

enum struct Case {
    DEFAULT,
    CAMEL,
    CAPITAL,
    CONSTANT,
    DOT,
    HEADER,
    NO,
    PARAM,
    PASCAL,
    PATH,
    SENTENCE,
    SNAKE,
    TITLE,
    SWAP,
    LOWER,
    LOWER_FIRST,
    UPPER,
    UPPER_FIRST,
    SPONGE,
};

Res<String> toDefaultCase(Str str);

Res<String> toCamelCase(Str str);

Res<String> toCapitalCase(Str str);

Res<String> toConstantCase(Str str);

Res<String> toDotCase(Str str);

Res<String> toHeaderCase(Str str);

Res<String> toNoCase(Str str);

Res<String> toParamCase(Str str);

Res<String> toPascalCase(Str str);

Res<String> toPathCase(Str str);

Res<String> toSentenceCase(Str str);

Res<String> toSnakeCase(Str str);

Res<String> toTitleCase(Str str);

Res<String> toSwapCase(Str str);

Res<String> toLowerCase(Str str);

Res<String> toLowerFirstCase(Str str);

Res<String> toUpperCase(Str str);

Res<String> toUpperFirstCase(Str str);

Res<String> toSpongeCase(Str str);

Res<String> changeCase(Str str, Case toCase);

template <typename T>
struct Cased {
    T _inner;
    Case _case;
};

inline auto cased(auto inner, Case cased) {
    return Cased<decltype(inner)>{inner, cased};
}

template <typename T>
struct Formatter<Cased<T>> {
    Formatter<T> _innerFmt{};

    void parse(Io::SScan &scan) {
        if constexpr (requires() {
                          _innerFmt.parse(scan);
                      }) {
            _innerFmt.parse(scan);
        }
    }

    Res<usize> format(Io::TextWriter &writer, Cased<T> val) {
        auto result = try$(changeCase(val._inner, val._case));
        return writer.writeStr(result);
    }
};

/* --- Number Formatting ---------------------------------------------------- */

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
            if (v < 10)
                return '0' + v;
            return 'a' + (v - 10);
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

/* --- Format Optionals ----------------------------------------------------- */

template <>
struct Formatter<None> {
    Res<usize> format(Io::TextWriter &writer, None const &) {
        return writer.writeStr("None");
    }
};

template <typename T>
struct Formatter<Opt<T>> {
    Formatter<T> formatter;

    void parse(Io::SScan &scan) {
        if constexpr (requires() {
                          formatter.parse(scan);
                      }) {
            formatter.parse(scan);
        }
    }

    Res<usize> format(Io::TextWriter &writer, Opt<T> const &val) {
        if (val)
            return formatter.format(writer, *val);
        return writer.writeStr("None");
    }
};

template <typename T>
struct Formatter<Ok<T>> {
    Formatter<T> formatter;

    void parse(Io::SScan &scan) {
        if constexpr (requires() {
                          formatter.parse(scan);
                      }) {
            formatter.parse(scan);
        }
    }

    Res<usize> format(Io::TextWriter &writer, Ok<T> const &val) {
        if constexpr (Meta::Same<T, None>)
            return writer.writeStr("Ok");
        else
            return formatter.format(writer, val.inner);
    }
};

template <>
struct Formatter<Error> {
    Res<usize> format(Io::TextWriter &writer, Error const &val) {
        usize written = 0;
        written += try$(writer.writeStr(val.msg()));
        return Ok(written);
    }
};

template <typename T, typename E>
struct Formatter<Res<T, E>> {
    Formatter<T> _fmtOk;
    Formatter<E> _fmtErr;

    void parse(Io::SScan &scan) {
        if constexpr (requires() {
                          _fmtOk.parse(scan);
                      }) {
            _fmtOk.parse(scan);
        }

        if constexpr (requires() {
                          _fmtErr.parse(scan);
                      }) {
            _fmtErr.parse(scan);
        }
    }

    Res<usize> format(Io::TextWriter &writer, Res<T> const &val) {
        if (val)
            return _fmtOk.format(writer, val.unwrap());
        return _fmtErr.format(writer, val.none());
    }
};

/* --- Format References ---------------------------------------------------- */

template <typename T>
struct Formatter<Strong<T>> {
    Formatter<T> formatter;

    void parse(Io::SScan &scan) {
        if constexpr (requires() {
                          formatter.parse(scan);
                      }) {
            formatter.parse(scan);
        }
    }

    Res<usize> format(Io::TextWriter &writer, Strong<T> const &val) {
        return formatter.format(writer, val.unwrap());
    }
};

template <typename T>
struct Formatter<Weak<T>> {
    Formatter<T> formatter;

    void parse(Io::SScan &scan) {
        if constexpr (requires() {
                          formatter.parse(scan);
                      }) {
            formatter.parse(scan);
        }
    }

    Res<usize> format(Io::TextWriter &writer, Weak<T> const &val) {
        auto inner = val.upgrade();
        if (not inner)
            return writer.writeStr("None");
        return formatter.format(writer, inner.unwrap().unwrap());
    }
};

/* --- Format Reflectable --------------------------------------------------- */

template <Reflectable T>
struct Formatter<T> {
    Res<usize> format(Io::TextWriter &writer, T const &val) {
        usize written = try$(Io::format(writer, "{}(", nameOf<T>()));

        bool first = true;
        try$(iterFields(val, [&](Str name, auto const &v) -> Res<usize> {
            if (not first)
                written += try$(writer.writeStr(", "));
            first = false;
            written += try$(Io::format(writer, "{}={}", name, v));
            return Ok(written);
        }));
        written += try$(writer.writeRune(')'));
        return Ok(written);
    }
};

/* --- Format Sliceable ----------------------------------------------------- */

template <Sliceable T>
struct Formatter<T> {
    Formatter<typename T::Inner> inner;

    void parse(Io::SScan &scan) {
        if constexpr (requires() {
                          inner.parse(scan);
                      }) {
            inner.parse(scan);
        }
    }

    Res<usize> format(Io::TextWriter &writer, T const &val) {
        auto written = try$(writer.writeStr("{"));
        for (usize i = 0; i < val.len(); i++) {
            if (i != 0)
                written += try$(writer.writeStr(", "));
            written += try$(inner.format(writer, val[i]));
        }
        return Ok(written + try$(writer.writeStr("}")));
    }
};

/* --- Format String -------------------------------------------------------- */

template <>
struct Formatter<Str> {
    Res<usize> format(Io::TextWriter &writer, Str text) {
        return writer.writeStr(text);
    }
};

template <>
struct Formatter<String> {
    Res<usize> format(Io::TextWriter &writer, String const &text) {
        return writer.writeStr(text);
    }
};

template <>
struct Formatter<char const *> : public Formatter<Str> {};

/* --- Format Time ---------------------------------------------------------- */

template <>
struct Formatter<TimeSpan> {
    Res<usize> format(Io::TextWriter &writer, TimeSpan const &val) {
        return Io::format(writer, "{}.{03}", val.toSecs(), val.toUSecs() % 1000);
    }
};

template <>
struct Formatter<TimeStamp> {
    Res<usize> format(Io::TextWriter &writer, TimeStamp const &val) {
        return Io::format(writer, "{}", DateTime::fromTimeStamp(val));
    }
};

template <>
struct Formatter<Time> {
    Res<usize> format(Io::TextWriter &writer, Time const &val) {
        return Io::format(writer, "{02}:{02}:{02}", val.hour, val.minute, val.second);
    }
};

template <>
struct Formatter<Date> {
    Res<usize> format(Io::TextWriter &writer, Date const &val) {
        return Io::format(writer, "{04}-{02}-{02}", (isize)val.year, (usize)val.month + 1, (usize)val.day + 1);
    }
};

template <>
struct Formatter<DateTime> {
    Res<usize> format(Io::TextWriter &writer, DateTime const &val) {
        return Io::format(writer, "{} {}", val.date, val.time);
    }
};

/* --- Format Tuple --------------------------------------------------------- */

template <typename Car, typename Cdr>
struct Formatter<Cons<Car, Cdr>> {

    Res<usize> format(Io::TextWriter &writer, Cons<Car, Cdr> const &val) {
        usize written = 0;
        written += try$(writer.writeRune('{'));

        Formatter<Car> carFormatter;
        written += try$(carFormatter.format(writer, val.car));
        written += try$(writer.writeStr(", "));

        Formatter<Cdr> cdrFormatter;
        written += try$(cdrFormatter.format(writer, val.cdr));
        written += try$(writer.writeRune('}'));
        return Ok(written);
    }
};

template <typename... Ts>
struct Formatter<Tuple<Ts...>> {

    Res<usize> format(Io::TextWriter &writer, Tuple<Ts...> const &val) {
        usize written = 0;
        bool first = true;
        written += try$(writer.writeRune('{'));
        try$(val.visit([&]<typename T>(T const &f) -> Res<usize> {
            if (not first)
                written += try$(writer.writeStr(", "));

            Formatter<T> formatter;
            written += try$(formatter.format(writer, f));

            first = false;
            return Ok(0uz);
        }));
        written += try$(writer.writeRune('}'));
        return Ok(written);
    }
};

} // namespace Karm::Io
