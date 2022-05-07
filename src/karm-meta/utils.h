#pragma once

namespace Karm::Meta {

struct NoCopy {
    NoCopy() = default;
    NoCopy(NoCopy const &) = delete;
    NoCopy &operator=(NoCopy const &) = delete;
};

struct NoMove {
    NoMove() = default;
    NoMove(NoMove &&) = delete;
    NoMove &operator=(NoMove &&) = delete;
};

template <typename T>
T declval();

} // namespace Karm::Meta
