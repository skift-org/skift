export module Karm.Core:io.types;

import :base.base;

namespace Karm::Io {

export enum struct Whence {
    BEGIN,
    CURRENT,
    END,
};

export struct Seek {
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

    Seek(Whence whence, isize offset = 0)
        : whence(whence), offset(offset) {}

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

    bool operator==(Whence const& other) const {
        return whence == other;
    }
};

} // namespace Karm::Io
