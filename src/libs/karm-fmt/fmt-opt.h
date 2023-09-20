#pragma once

#include "base.h"

namespace Karm::Fmt {

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

    Res<usize> format(Io::TextWriter &writer, Opt<T> opt) {
        if (opt) {
            return formatter.format(writer, *opt);
        }
        return writer.writeStr("<None>");
    }
};

} // namespace Karm::Fmt
