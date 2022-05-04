#pragma once

#include <karm-base/result.h>
#include <karm-base/tuple.h>
#include <karm-io/traits.h>
#include <karm-text/str.h>

namespace Karm::Fmt {

struct Fmt {
    enum Type {
        NONE,
        CHAR,
        STRING,
        BINARY,
        OCTAL,
        DECIMAL,
        HEXADECIMAL,
        POINTER,
    } type;
};

struct _Args {
    virtual size_t len() = 0;
    virtual Base::Result<size_t> format(Fmt &fmt, Io::Writer &writer, size_t index) = 0;
};

template <typename... Ts>
struct Args : public _Args {
    Tuple<Ts...> _tuple;

    Args(Ts &&...ts) : _tuple(std::forward<Ts>(ts)...) {}

    size_t len() override {
        return _tuple.len();
    }

    Base::Result<size_t> format(Fmt &fmt, Io::Writer &writer, size_t index) override {
        Base::Result<size_t> result = {0};
        return _tuple.visit([&](auto const &t) {
            if (index == 0) {
                result = format(fmt, writer, t);
            } else {
                index--;
            }
        });
        return result;
    }
};

Base::Result<size_t> _format(Io::Writer &writer, Text::Str str, _Args &args);

template <typename... Ts>
Base::Result<size_t> format(Io::Writer &writer, Text::Str str, Ts &&...ts) {
    Args<Ts...> args{Tuple<Ts...>{std::forward<Ts>(ts)...}};
    return _format(writer, str, args);
}

} // namespace Karm::Fmt
