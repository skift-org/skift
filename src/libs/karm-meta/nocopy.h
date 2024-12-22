#pragma once

namespace Karm::Meta {

/// A type that can't be copied.
struct NoCopy {
    NoCopy() = default;
    NoCopy(NoCopy const &) = delete;
    NoCopy &operator=(NoCopy const &) = delete;
    NoCopy(NoCopy &&) = default;
    NoCopy &operator=(NoCopy &&) = default;
};

/// A type that can't be moved or copied.
struct Pinned {
    Pinned() = default;
    Pinned(Pinned &&) = delete;
    Pinned &operator=(Pinned &&) = delete;
};

} // namespace Karm::Meta
