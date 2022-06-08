#pragma once

namespace Karm::Meta {

struct NoCopy {
    NoCopy() = default;
    NoCopy(NoCopy const &) = delete;
    NoCopy &operator=(NoCopy const &) = delete;
    NoCopy(NoCopy &&) = default;
    NoCopy &operator=(NoCopy &&) = default;
};

struct Static {
    Static() = default;
    Static(Static &&) = delete;
    Static &operator=(Static &&) = delete;
};

template <typename T>
T declval();

template <typename T>
T &declref();

} // namespace Karm::Meta
