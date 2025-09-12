module;

#include <karm-core/macros.h>

export module Karm.Core:io.fmt;

import :base.box;
import :base.cow;
import :base.endian;
import :base.enum_;
import :base.map;
import :base.rc;
import :base.symbol;
import :base.time;
import :base.vec;
import :io.aton;
import :io.sscan;
import :io.text;

namespace Karm::Io {

export template <typename T>
struct Formatter;

export struct _Args {
    virtual ~_Args() = default;
    virtual usize len() = 0;
    virtual Res<> format(SScan& scan, TextWriter& writer, usize index) = 0;
};

export template <typename... Ts>
struct Args : _Args {
    Tuple<Ts...> _tuple{};

    Args(Ts&&... ts) : _tuple(std::forward<Ts>(ts)...) {}

    usize len() override {
        return _tuple.len();
    }

    Res<> format(SScan& scan, TextWriter& writer, usize index) override {
        Res<> result = Error::invalidData("format index out of range");
        usize i = 0;
        _tuple.visit([&](auto const& t) {
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

export Res<> _format(TextWriter& writer, Str format, _Args& args) {
    SScan scan{format};
    usize index = 0;

    while (not scan.ended()) {
        Rune c = scan.next();

        if (c == '{') {
            scan.skip(':');
            scan.begin();
            while (scan.peek() != '}') {
                scan.next();
            }
            scan.next();
            SScan inner{scan.end()};
            try$(args.format(inner, writer, index));
            index++;
        } else {
            try$(writer.writeRune(c));
        }
    }

    return Ok();
};

export Res<> format(TextWriter& writer, Str format) {
    return writer.writeStr(format);
}

export template <typename... Ts>
Res<> format(TextWriter& writer, Str format, Ts&&... ts) {
    Args<Ts...> args{std::forward<Ts>(ts)...};
    return _format(writer, format, args);
}

export String format(Str format) {
    return format;
}

export template <typename... Ts>
String format(Str format, Ts&&... ts) {
    StringWriter writer{};
    Args<Ts...> args{std::forward<Ts>(ts)...};
    _format(writer, format, args).unwrap("formating string");
    return writer.take();
}

export template <typename T>
String toStr(T const& t, Str format = "") {
    StringWriter writer{};
    Formatter<T> formatter;
    if constexpr (requires(SScan& scan) {
                      formatter.parse(scan);
                  }) {
        SScan scan{format};
        formatter.parse(scan);
    }
    formatter.format(writer, t).unwrap("formating string");
    return writer.take();
}

// MARK: Align Formatting ------------------------------------------------------

export enum struct Align {
    LEFT,
    RIGHT,
    CENTER,
};

export template <typename T>
struct Aligned {
    T _inner;
    Align _align;
    usize _width;
};

export auto aligned(auto inner, Align align, usize width) {
    return Aligned<decltype(inner)>{inner, align, width};
}

export template <typename T>
struct Formatter<Aligned<T>> {
    Formatter<T> _innerFmt{};

    void parse(SScan& scan) {
        if constexpr (requires() {
                          _innerFmt.parse(scan);
                      }) {
            _innerFmt.parse(scan);
        }
    }

    Res<> format(TextWriter& writer, Aligned<T> val) {
        StringWriter buf;
        try$(_innerFmt.format(buf, val._inner));
        usize width = buf.len();

        if (width < val._width) {
            usize pad = val._width - width;
            switch (val._align) {
            case Align::LEFT:
                try$(writer.writeStr(buf.str()));
                for (usize i = 0; i < pad; i++)
                    try$(writer.writeRune(' '));
                break;
            case Align::RIGHT:
                for (usize i = 0; i < pad; i++)
                    try$(writer.writeRune(' '));
                try$(writer.writeStr(buf.str()));
                break;
            case Align::CENTER:
                for (usize i = 0; i < pad / 2; i++)
                    try$(writer.writeRune(' '));
                try$(writer.writeStr(buf.str()));
                for (usize i = 0; i < pad / 2; i++)
                    try$(writer.writeRune(' '));
                break;
            }
        } else {
            try$(writer.writeStr(buf.str()));
        }

        return Ok();
    }
};

// MARK: Number Formatting -----------------------------------------------------

export struct NumberFormatter {
    bool prefix = false;
    bool isChar = false;
    usize base = 10;
    usize width = 0;
    char fillChar = ' ';
    bool trailingZeros = false;
    usize precision = 6;

    Str formatPrefix() {
        if (base == 16)
            return "0x";

        if (base == 8)
            return "0o";

        if (base == 2)
            return "0b";

        return "";
    }

    void parse(Str str) {
        SScan scan{str};
        parse(scan);
    }

    void parse(SScan& s) {
        if (s.skip('#'))
            prefix = true;

        if (s.skip('0'))
            fillChar = '0';

        width = atoi(s).unwrapOr(0);

        if (s.skip('.')) {
            if (s.skip('0'))
                trailingZeros = true;
            precision = atoi(s).unwrapOrDefault(6);
        }

        if (s.ended())
            return;
        Rune c = s.next();
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
            prefix = true;
            base = 16;
            fillChar = '0';
            width = sizeof(usize) * 2;
            break;

        case 'c':
            isChar = true;
            break;

        default:
            break;
        }
    }

    Res<> formatUnsigned(TextWriter& writer, usize val) {
        auto digit = [](usize v) {
            if (v < 10)
                return '0' + v;
            return 'a' + (v - 10);
        };

        InlineVec<char, 128> buf;

        do {
            buf.pushBack(digit(val % base));
            val /= base;
        } while (val != 0 and buf.len() < buf.cap());

        while (width > buf.len())
            buf.pushBack(fillChar);

        reverse(mutSub(buf));

        if (prefix)
            try$(writer.writeStr(formatPrefix()));
        try$(writer.writeStr(Str{buf}));

        return Ok();
    }

    Res<> formatSigned(TextWriter& writer, isize val) {
        if (val < 0) {
            try$(writer.writeRune('-'));
            val = -val;
        }
        try$(formatUnsigned(writer, val));
        return Ok();
    }

#ifndef __ck_freestanding__
    Res<> formatFloat(TextWriter& writer, f64 val) {
        NumberFormatter formatter;
        isize ipart = (isize)val;
        f64 fpart = Math::abs(val - (f64)ipart);
        u64 ifpart = (u64)(fpart * Math::pow(10uz, precision));

        if (val < 0) {
            try$(writer.writeRune('-'));
            ipart = -ipart;
        }
        try$(formatUnsigned(writer, ipart));

        if ((ifpart != 0 or trailingZeros) and precision > 0) {
            try$(writer.writeRune('.'));
            formatter.width = precision;
            formatter.fillChar = '0';
            try$(formatter.formatUnsigned(writer, ifpart));
        }
        return Ok();
    }
#endif

    Res<> formatRune(TextWriter& writer, Rune val) {
        if (not prefix)
            return writer.writeRune(val);

        if (val == '\'')
            return writer.writeStr("\\'"s);

        if (val == '\"')
            return writer.writeStr("\\\""s);

        if (val == '\?')
            return writer.writeStr("\\?"s);

        if (val == '\\')
            return writer.writeStr("\\\\"s);

        if (val == '\a')
            return writer.writeStr("\\a"s);

        if (val == '\b')
            return writer.writeStr("\\b"s);

        if (val == '\f')
            return writer.writeStr("\\f"s);

        if (val == '\n')
            return writer.writeStr("\\n"s);

        if (val == '\r')
            return writer.writeStr("\\r"s);

        if (val == '\t')
            return writer.writeStr("\\t"s);

        if (val == '\v')
            return writer.writeStr("\\v"s);

        if (not isAsciiPrint(val))
            return format(writer, "\\u{x}", val);

        return writer.writeRune(val);
    }
};

export template <Meta::UnsignedIntegral T>
struct Formatter<T> : NumberFormatter {
    Res<> format(TextWriter& writer, T const& val) {
        if (isChar)
            return formatRune(writer, val);
        return formatUnsigned(writer, val);
    }
};

export template <Meta::SignedIntegral T>
struct Formatter<T> : NumberFormatter {
    Res<> format(TextWriter& writer, T const& val) {
        if (isChar)
            return writer.writeRune(val);
        return formatSigned(writer, val);
    }
};

#ifndef __ck_freestanding__
export template <Meta::Float T>
struct Formatter<T> : NumberFormatter {
    Res<> format(TextWriter& writer, f64 const& val) {
        return formatFloat(writer, val);
    }
};
#endif

export template <typename T>
struct Formatter<Be<T>> : Formatter<T> {
    Res<> format(TextWriter& writer, Be<T> const& val) {
        return Formatter<T>::format(writer, val.value());
    }
};

export template <typename T>
struct Formatter<Le<T>> : Formatter<T> {
    Res<> format(TextWriter& writer, Le<T> const& val) {
        return Formatter<T>::format(writer, val.value());
    }
};

// MARK: Format Enums ----------------------------------------------------------

export template <Meta::Enum T>
struct Formatter<T> {
    Res<> format(TextWriter& writer, T val) {
        if constexpr (BoundedEnum<T>) {
            return writer.writeStr(nameOf<T>(val));
        } else {
            return Io::format(writer, "({} {})", nameOf<T>(), toUnderlyingType(val));
        }
    }
};

// MARK: Format Pointers -------------------------------------------------------

export template <typename T>
struct Formatter<T*> {
    bool prefix = false;

    void parse(SScan& scan) {
        prefix = scan.skip('#');
    }

    Res<> format(TextWriter& writer, T* val) {
        if (prefix) {
            try$(writer.writeRune('('));
            try$(writer.writeStr(nameOf<T>()));
            try$(writer.writeStr(" *)"s));
        }

        if (val) {
            NumberFormatter fmt;
            fmt.base = 16;
            fmt.fillChar = '0';
            fmt.width = sizeof(T*) * 2;
            fmt.prefix = true;
            try$(fmt.formatUnsigned(writer, (usize)val));
        } else {
            try$(writer.writeStr("nullptr"s));
        }

        return Ok();
    }
};

export template <>
struct Formatter<std::nullptr_t> {
    Res<> format(TextWriter& writer, std::nullptr_t) {
        return writer.writeStr("nullptr"s);
    }
};

// MARK: Format Optionals ------------------------------------------------------

export template <>
struct Formatter<None> {
    Res<> format(TextWriter& writer, None const&) {
        return writer.writeStr("None"s);
    }
};

export template <>
struct Formatter<bool> {
    Res<> format(TextWriter& writer, bool val) {
        return writer.writeStr(val ? "True"s : "False"s);
    }
};

export template <typename T>
struct Formatter<Opt<T>> {
    Formatter<T> formatter;

    void parse(SScan& scan) {
        if constexpr (requires() {
                          formatter.parse(scan);
                      }) {
            formatter.parse(scan);
        }
    }

    Res<> format(TextWriter& writer, Opt<T> const& val) {
        if (val)
            return formatter.format(writer, *val);
        return writer.writeStr("None"s);
    }
};

// MARK: Format Results --------------------------------------------------------

export template <typename T>
struct Formatter<Ok<T>> {
    Formatter<T> formatter;

    void parse(SScan& scan) {
        if constexpr (requires() {
                          formatter.parse(scan);
                      }) {
            formatter.parse(scan);
        }
    }

    Res<> format(TextWriter& writer, Ok<T> const& val) {
        if constexpr (Meta::Same<T, None>)
            return writer.writeStr("Ok"s);
        else
            return formatter.format(writer, val.inner);
    }
};

export template <>
struct Formatter<Error> {
    Res<> format(TextWriter& writer, Error const& val) {
        Str msg = Str::fromNullterminated(val.msg());
        try$(writer.writeStr(msg));
        return Ok();
    }
};

export template <typename T, typename E>
struct Formatter<Res<T, E>> {
    Formatter<T> _fmtOk;
    Formatter<E> _fmtErr;

    void parse(SScan& scan) {
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

    Res<> format(TextWriter& writer, Res<T, E> const& val) {
        if (val)
            return _fmtOk.format(writer, val.unwrap());
        return _fmtErr.format(writer, val.none());
    }
};

// MARK: Format Unions ---------------------------------------------------------

export template <typename... Ts>
struct Formatter<Union<Ts...>> {
    Res<> format(TextWriter& writer, Union<Ts...> const& val) {
        return val.visit([&](auto const& v) -> Res<> {
            return Io::format(writer, "{}", v);
        });
    }
};

// MARK: Format Ordering -------------------------------------------------------

export template <>
struct Formatter<std::strong_ordering> {
    Res<> format(TextWriter& writer, std::strong_ordering val) {
        if (val == std::strong_ordering::less)
            return writer.writeStr("Less"s);

        if (val == std::strong_ordering::greater)
            return writer.writeStr("Greater"s);

        return writer.writeStr("Equal"s);
    }
};

export template <>
struct Formatter<std::weak_ordering> {
    Res<> format(TextWriter& writer, std::weak_ordering val) {
        if (val == std::weak_ordering::less)
            return writer.writeStr("Less"s);

        if (val == std::weak_ordering::greater)
            return writer.writeStr("Greater"s);

        return writer.writeStr("Equivalent"s);
    }
};

export template <>
struct Formatter<std::partial_ordering> {
    Res<> format(TextWriter& writer, std::partial_ordering val) {
        if (val == std::partial_ordering::equivalent)
            return writer.writeStr("Equivalent"s);

        if (val == std::partial_ordering::less)
            return writer.writeStr("Less"s);

        if (val == std::partial_ordering::greater)
            return writer.writeStr("Greater"s);

        return writer.writeStr("Unordered"s);
    }
};

// MARK: Format References -----------------------------------------------------

export template <typename T>
struct Formatter<Rc<T>> {
    Formatter<T> formatter;

    void parse(SScan& scan) {
        if constexpr (requires() {
                          formatter.parse(scan);
                      }) {
            formatter.parse(scan);
        }
    }

    Res<> format(TextWriter& writer, Rc<T> const& val) {
        return formatter.format(writer, val.unwrap());
    }
};

export template <typename T>
struct Formatter<Weak<T>> {
    Formatter<T> formatter;

    void parse(SScan& scan) {
        if constexpr (requires() {
                          formatter.parse(scan);
                      }) {
            formatter.parse(scan);
        }
    }

    Res<> format(TextWriter& writer, Weak<T> const& val) {
        auto inner = val.upgrade();
        if (not inner)
            return writer.writeStr("None"s);
        return formatter.format(writer, inner.unwrap().unwrap());
    }
};

export template <typename T>
struct Formatter<Box<T>> {
    Formatter<T> formatter;

    void parse(SScan& scan) {
        if constexpr (requires() {
                          formatter.parse(scan);
                      }) {
            formatter.parse(scan);
        }
    }

    Res<> format(TextWriter& writer, Box<T> const& val) {
        return formatter.format(writer, *val);
    }
};

export template <typename T>
struct Formatter<Cow<T>> {
    Formatter<T> formatter;

    void parse(SScan& scan) {
        if constexpr (requires() {
                          formatter.parse(scan);
                      }) {
            formatter.parse(scan);
        }
    }

    Res<> format(TextWriter& writer, Cow<T> const& val) {
        return formatter.format(writer, *val);
    }
};

// MARK: Format Reflectable ----------------------------------------------------

export struct Emit;

export template <typename T>
struct Repr;

template <typename T>
concept ReprMethod = requires(T t, Emit& emit) {
    t.repr(emit);
};

export template <typename T>
concept Reprable =
    ReprMethod<T> or
    requires(T t, Emit& emit) {
        Repr<T>::repr(emit, t);
    };

// MARK: Format Sliceable ------------------------------------------------------

export template <Sliceable T>
struct Formatter<T> {
    Formatter<typename T::Inner> inner;

    void parse(SScan& scan) {
        if constexpr (requires() {
                          inner.parse(scan);
                      }) {
            inner.parse(scan);
        }
    }

    Res<> format(TextWriter& writer, T const& val) {
        try$(writer.writeStr("["s));
        for (usize i = 0; i < val.len(); i++) {
            if (i != 0)
                try$(writer.writeStr(", "s));
            try$(inner.format(writer, val[i]));
        }
        return Ok(try$(writer.writeStr("]"s)));
    }
};

// MARK: Format Map ------------------------------------------------------------

export template <typename K, typename V>
struct Formatter<Map<K, V>> {
    Formatter<K> keyFormatter;
    Formatter<V> valFormatter;

    void parse(SScan& scan) {
        if constexpr (requires() {
                          keyFormatter.parse(scan);
                          valFormatter.parse(scan);
                      }) {
            keyFormatter.parse(scan);
            valFormatter.parse(scan);
        }
    }

    Res<> format(TextWriter& writer, Map<K, V> const& val) {
        try$(writer.writeStr("{"s));
        bool first = true;
        for (auto const& [key, value] : val.iter()) {
            if (not first)
                try$(writer.writeStr(", "s));
            first = false;
            try$(keyFormatter.format(writer, key));
            try$(writer.writeStr(": "s));
            try$(valFormatter.format(writer, value));
        }
        try$(writer.writeStr("}"s));
        return Ok();
    }
};

// MARK: Format Range ----------------------------------------------------------

export template <typename T, typename Tag>
struct Formatter<Range<T, Tag>> {

    Formatter<T> inner;

    void parse(SScan& scan) {
        if constexpr (requires() {
                          inner.parse(scan);
                      }) {
            inner.parse(scan);
        }
    }

    Res<> format(TextWriter& writer, Range<T, Tag> const& val) {
        try$(writer.writeStr("["s));
        try$(inner.format(writer, val.start));
        try$(writer.writeStr("-"s));
        try$(inner.format(writer, val.end()));
        try$(writer.writeStr("]"s));
        return Ok();
    }
};

// MARK: Format String ---------------------------------------------------------

export template <StaticEncoding E>
struct StringFormatter {
    bool prefix = false;

    void parse(SScan& scan) {
        if (scan.skip('#'))
            prefix = true;
    }

    Res<> format(TextWriter& writer, _Str<E> text) {
        if (not prefix)
            return writer.writeStr(text);

        try$(writer.writeRune('"'));
        for (Rune c : iterRunes(text)) {
            if (c == '"')
                try$(writer.writeStr("\\\""s));
            else if (c == '\\')
                try$(writer.writeStr("\\\\"s));
            else if (c == '\a')
                try$(writer.writeStr("\\a"s));
            else if (c == '\b')
                try$(writer.writeStr("\\b"s));
            else if (c == '\f')
                try$(writer.writeStr("\\f"s));
            else if (c == '\n')
                try$(writer.writeStr("\\n"s));
            else if (c == '\r')
                try$(writer.writeStr("\\r"s));
            else if (c == '\t')
                try$(writer.writeStr("\\t"s));
            else if (c == '\v')
                try$(writer.writeStr("\\v"s));
            else if (not isAsciiPrint(c))
                try$(Io::format(writer, "\\u{x}", c));
            else
                try$(writer.writeRune(c));
        }
        try$(writer.writeRune('"'));
        return Ok();
    }
};

export template <StaticEncoding E>
struct Formatter<_Str<E>> : StringFormatter<E> {};

export template <StaticEncoding E>
struct Formatter<_String<E>> : StringFormatter<E> {
    Res<> format(TextWriter& writer, _String<E> const& text) {
        return StringFormatter<E>::format(writer, text.str());
    }
};

export template <usize N>
struct Formatter<StrLit<N>> : StringFormatter<Utf8> {};

export template <>
struct Formatter<char const*> : StringFormatter<Utf8> {
    Res<> format(TextWriter& writer, char const* text) {
        _Str<Utf8> str = Str::fromNullterminated(text);
        return StringFormatter::format(writer, str);
    }
};

export template <>
struct Formatter<Symbol> : StringFormatter<Utf8> {
    Res<> format(TextWriter& writer, Symbol const& text) {
        return StringFormatter::format(writer, text.str());
    }
};

// MARK: Format Time -----------------------------------------------------------

export template <>
struct Formatter<Duration> {
    Res<> format(TextWriter& writer, Duration const& val) {
        return Io::format(writer, "{}.{03}s", val.toSecs(), val.toMSecs() % 1000);
    }
};

export template <>
struct Formatter<Instant> {
    Res<> format(TextWriter& writer, Instant const& val) {
        return Io::format(writer, "monotonic:{}", val._value);
    }
};

export template <>
struct Formatter<SystemTime> {
    Res<> format(TextWriter& writer, SystemTime const& val) {
        return Io::format(writer, "{}", DateTime::fromInstant(val));
    }
};

export template <>
struct Formatter<Time> {
    Res<> format(TextWriter& writer, Time const& val) {
        return Io::format(writer, "{02}:{02}:{02}", val.hour, val.minute, val.second);
    }
};

export template <>
struct Formatter<Date> {
    Res<> format(TextWriter& writer, Date const& val) {
        return Io::format(writer, "{04}-{02}-{02}", (isize)val.year, (usize)val.month + 1, (usize)val.day + 1);
    }
};

export template <>
struct Formatter<DateTime> {
    Res<> format(TextWriter& writer, DateTime const& val) {
        return Io::format(writer, "{} {}", val.date, val.time);
    }
};

// MARK: Format Tuple ----------------------------------------------------------

export template <typename Car, typename Cdr>
struct Formatter<Pair<Car, Cdr>> {
    Res<> format(TextWriter& writer, Pair<Car, Cdr> const& val) {
        try$(writer.writeRune('{'));

        Formatter<Car> carFormatter;
        try$(carFormatter.format(writer, val.v0));
        try$(writer.writeStr(", "s));

        Formatter<Cdr> cdrFormatter;
        try$(cdrFormatter.format(writer, val.v1));
        try$(writer.writeRune('}'));
        return Ok();
    }
};

export template <typename... Ts>
struct Formatter<Tuple<Ts...>> {
    Res<> format(TextWriter& writer, Tuple<Ts...> const& val) {
        bool first = true;
        try$(writer.writeRune('{'));
        try$(val.visit([&]<typename T>(T const& f) -> Res<> {
            if (not first)
                try$(writer.writeStr(", "s));

            Formatter<T> formatter;
            try$(formatter.format(writer, f));

            first = false;
            return Ok();
        }));
        try$(writer.writeRune('}'));
        return Ok();
    }
};

} // namespace Karm::Io
