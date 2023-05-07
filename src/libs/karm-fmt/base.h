#pragma once

#include <karm-base/tuple.h>
#include <karm-io/impls.h>
#include <karm-io/traits.h>
#include <karm-text/scan.h>

namespace Karm::Fmt {

template <typename T>
struct Formatter;

struct _Args {
    virtual ~_Args() = default;
    virtual usize len() = 0;
    virtual Res<usize> format(Text::Scan &scan, Io::TextWriter &writer, usize index) = 0;
};

template <typename... Ts>
struct Args : public _Args {
    Tuple<Ts...> _tuple{};

    Args(Ts &&...ts) : _tuple(std::forward<Ts>(ts)...) {}

    usize len() override { return _tuple.len(); }

    Res<usize> format(Text::Scan &scan, Io::TextWriter &writer, usize index) override {
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

inline Res<usize> _format(Io::TextWriter &writer, Str format, _Args &args) {
    Text::Scan scan{format};
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
