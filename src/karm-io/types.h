#pragma once

#include <karm-base/macros.h>
#include <karm-base/result.h>
#include <karm-meta/same.h>

namespace Karm::Io {

struct Seek {
    enum class Whence {
        BEGIN,
        CURRENT,
        END,
    };

    Whence whence;
    ssize_t offset;

    static auto from_begin(ssize_t offset) -> Seek {
        return Seek{Whence::BEGIN, offset};
    }

    static auto from_current(ssize_t offset) -> Seek {
        return Seek{Whence::CURRENT, offset};
    }

    static Seek from_end(ssize_t offset) {
        return Seek{Whence::END, offset};
    }

    auto apply(ssize_t current) const -> size_t {
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
