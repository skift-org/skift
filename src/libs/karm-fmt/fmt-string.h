#pragma once

#include "base.h"

namespace Karm::Fmt {

template <>
struct Formatter<Str> {
    Res<usize> format(Io::TextWriter &writer, Str text) {
        return writer.writeStr(text);
    }
};

template <>
struct Formatter<String> {
    Res<usize> format(Io::TextWriter &writer, String text) {
        return writer.writeStr(text);
    }
};

template <>
struct Formatter<char const *> : public Formatter<Str> {};

} // namespace Karm::Fmt
