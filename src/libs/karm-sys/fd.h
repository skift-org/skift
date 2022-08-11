#pragma once

#include <karm-base/rc.h>
#include <karm-io/types.h>
#include <karm-meta/traits.h>
#include <karm-meta/utils.h>

#include "path.h"

namespace Karm::Sys {

struct Fd : Meta::NoCopy {
    virtual ~Fd() = default;

    virtual Result<size_t> read(MutBytes) = 0;

    virtual Result<size_t> write(Bytes) = 0;

    virtual Result<size_t> seek(Io::Seek seek) = 0;

    virtual Result<size_t> flush() = 0;

    virtual Result<Strong<Fd>> dup() = 0;
};

struct DummyFd : public Fd {
    Result<size_t> read(MutBytes) override {
        return 0;
    }

    Result<size_t> write(Bytes) override {
        return 0;
    }

    Result<size_t> seek(Io::Seek) override {
        return 0;
    }

    Result<size_t> flush() override {
        return 0;
    }

    Result<Strong<Fd>> dup() override {
        return {makeStrong<DummyFd>()};
    }
};

template <typename T>
concept AsFd = requires(T t) {
    { t.asFd() } -> Meta::Same<Strong<Fd>>;
};

} // namespace Karm::Sys
