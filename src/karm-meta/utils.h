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

struct Stone : NoCopy, NoMove {
};

template <typename T>
T declval();

template <typename T>
T &declref();

} // namespace Karm::Meta
