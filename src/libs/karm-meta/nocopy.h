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
struct Static {
    Static() = default;
    Static(Static &&) = delete;
    Static &operator=(Static &&) = delete;
};

} // namespace Karm::Meta
