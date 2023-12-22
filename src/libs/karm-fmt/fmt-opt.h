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

    Res<usize> format(Io::TextWriter &writer, Opt<T> const &val) {
        if (val)
            return formatter.format(writer, *val);
        return writer.writeStr("None");
    }
};

template <>
struct Formatter<Error> {
    Res<usize> format(Io::TextWriter &writer, Error const &val) {
        return writer.writeStr(val.msg());
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

} // namespace Karm::Fmt
