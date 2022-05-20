#pragma once

#include <karm-base/string.h>
#include <karm-fmt/fmt.h>
#include <karm-io/funcs.h>
#include <karm-io/traits.h>

namespace Karm::Text {

struct Emit {
    Io::Writer &_writer;
    size_t _total = 0;
    Error _error = OK;

    void operator()(Rune c) {
        if (!_error)
            return;

        Result<size_t> res = Io::putr(_writer, c);

        if (res) {
            _total += res.unwrap();
        } else {
            _error = res.none();
        }
    }

    void operator()(Str str) {
        if (!_error)
            return;

        Result<size_t> res = _writer.write(str.buf(), str.len());
        if (res) {
            _total += res.unwrap();
        } else {
            _error = res.none();
        }
    }

    template <typename... Ts>
    void operator()(Str format, Ts &&...ts) {
        if (!_error)
            return;

        Result<size_t> res = Fmt::format(_writer, format, std::forward<Ts>(ts)...);
        if (res) {
            _total += res.unwrap();
        } else {
            _error = res.none();
        }
    }

    size_t total() {
        return _total;
    }
};

} // namespace Karm::Text
