#pragma once

#include <karm-base/res.h>
#include <karm-base/try.h>
#include <karm-meta/traits.h>

namespace Karm::Io {

enum struct Whence {
    BEGIN,
    CURRENT,
    END,
};

struct Seek {
    Whence whence;
    isize offset;

    static constexpr Seek fromBegin(isize offset) {
        return Seek{Whence::BEGIN, offset};
    }

    static constexpr Seek fromCurrent(isize offset) {
        return Seek{Whence::CURRENT, offset};
    }

    static constexpr Seek fromEnd(isize offset) {
        return Seek{Whence::END, offset};
    }

    constexpr usize apply(usize current, usize size) const {
        switch (whence) {
        case Whence::BEGIN:
            return offset;

        case Whence::CURRENT:
            return current + offset;

        case Whence::END:
            return size - offset;
        }
    }
};

} // namespace Karm::Io
