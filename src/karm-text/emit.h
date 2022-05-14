#pragma once

#include <karm-base/string.h>
#include <karm-io/funcs.h>
#include <karm-io/traits.h>

namespace Karm::Text {

struct Emit {
    Io::Writer &_writer;
    size_t _total = 0;

    void operator()(Base::Rune c) {
        _total += Io::putr(_writer, c);
    }

    void operator()(Base::Str str) {
        _total += _writer.write(str.buf(), str.len());
    }

    size_t total() {
        return _total;
    }
};

} // namespace Karm::Text
