#pragma once

#include "string.h"
#include "vec.h"

namespace Karm {

struct Backtrace {
    struct Frame {
        String desc;
        String file;
        usize line;

        bool operator==(Frame const &other) const = default;

        auto operator<=>(Frame const &other) const = default;
    };

    enum struct Status {
        UNSUPPORTED,
        DISABLED,
        CAPTURED,
    };

    using enum Status;

    Vec<Frame> _frames;
    Status _status;

    Backtrace(Status status) : _status(status) {}

    static Backtrace capture();

    static Backtrace forceCapture();

    Slice<Frame> frames() const {
        return _frames;
    }

    Status status() const {
        return _status;
    }

    bool operator==(Backtrace const &other) const = default;

    auto operator<=>(Backtrace const &other) const = default;
};

} // namespace Karm
