#pragma once

#include <karm-base/rc.h>
#include <karm-io/types.h>
#include <karm-meta/nocopy.h>
#include <karm-meta/traits.h>

#include "path.h"

namespace Karm::Sys {

struct Fd : Meta::NoCopy {
    virtual ~Fd() = default;

    virtual Res<size_t> read(MutBytes) = 0;

    virtual Res<size_t> write(Bytes) = 0;

    virtual Res<size_t> seek(Io::Seek seek) = 0;

    virtual Res<size_t> flush() = 0;

    virtual Res<Strong<Fd>> dup() = 0;
};

struct DummyFd : public Fd {
    Res<size_t> read(MutBytes) override {
        return Ok(0uz);
    }

    Res<size_t> write(Bytes) override {
        return Ok(0uz);
    }

    Res<size_t> seek(Io::Seek) override {
        return Ok(0uz);
    }

    Res<size_t> flush() override {
        return Ok(0uz);
    }

    Res<Strong<Fd>> dup() override {
        return Ok(makeStrong<DummyFd>());
    }
};

template <typename T>
concept AsFd = requires(T t) {
                   { t.asFd() } -> Meta::Same<Strong<Fd>>;
               };

} // namespace Karm::Sys
