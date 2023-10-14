#pragma once

#include <karm-base/reflect.h>

#include "base.h"

namespace Karm::Fmt {

template <Reflectable T>
struct Formatter<T> {
    Res<usize> format(Io::TextWriter &writer, T const &v) {
        usize written = try$(Fmt::format(writer, "{}(", nameOf<T>()));

        bool first = true;
        try$(iterFields(v, [&](Str name, auto const &v) -> Res<usize> {
            if (not first)
                written += try$(writer.writeStr(", "));
            first = false;
            written += try$(Fmt::format(writer, "{}={}", name, v));
            return Ok(written);
        }));
        written += try$(writer.writeRune(')'));
        return Ok(written);
    }
};

} // namespace Karm::Fmt
