#pragma once

#include <karm-base/rc.h>
#include <karm-io/types.h>
#include <karm-meta/nocopy.h>
#include <karm-meta/traits.h>
#include <url/url.h>

namespace Karm::Sys {

struct Fd : Meta::NoCopy {
    virtual ~Fd() = default;

    virtual Res<usize> read(MutBytes) = 0;

    virtual Res<usize> write(Bytes) = 0;

    virtual Res<usize> seek(Io::Seek seek) = 0;

    virtual Res<usize> flush() = 0;

    virtual Res<Strong<Fd>> dup() = 0;
};

struct DummyFd : public Fd {
    Res<usize> read(MutBytes) override {
        return Ok(0uz);
    }

    Res<usize> write(Bytes) override {
        return Ok(0uz);
    }

    Res<usize> seek(Io::Seek) override {
        return Ok(0uz);
    }

    Res<usize> flush() override {
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
