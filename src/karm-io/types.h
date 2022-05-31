#pragma once

#include <karm-base/result.h>
#include <karm-base/try.h>
#include <karm-meta/traits.h>

namespace Karm::Io {

enum class Whence {
    BEGIN,
    CURRENT,
    END,
};

struct Seek {
    Whence whence;
    ssize_t offset;

    static constexpr Seek fromBegin(ssize_t offset) {
        return Seek{Whence::BEGIN, offset};
    }

    static constexpr Seek fromCurrent(ssize_t offset) {
        return Seek{Whence::CURRENT, offset};
    }

    static constexpr Seek fromEnd(ssize_t offset) {
        return Seek{Whence::END, offset};
    }

    constexpr size_t apply(ssize_t current) const {
        switch (whence) {
        case Whence::BEGIN:
            return offset;

        case Whence::CURRENT:
            return current + offset;

        case Whence::END:
            return -offset;
        }
    }
};

} // namespace Karm::Io
