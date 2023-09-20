#pragma once

#include "base.h"

namespace Karm::Fmt {

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

    Res<usize> format(Io::TextWriter &writer, T const &slice) {
        auto written = try$(writer.writeStr("{"));
        for (usize i = 0; i < slice.len(); i++) {
            if (i != 0)
                written += try$(writer.writeStr(", "));
            written += try$(inner.format(writer, slice[i]));
        }
        return Ok(written + try$(writer.writeStr("}")));
    }
};

} // namespace Karm::Fmt
