#pragma once

#include <karm-base/rc.h>
#include <karm-io/types.h>
#include <karm-meta/nocopy.h>
#include <karm-meta/traits.h>
#include <url/url.h>

#include "addr.h"

namespace Karm::Sys {

struct Stat {
    usize size;
    TimeStamp accessTime;
    TimeStamp modifyTime;
    TimeStamp changeTime;
};

struct Fd : Meta::NoCopy {
    virtual ~Fd() = default;

    virtual Res<usize> read(MutBytes) = 0;

    virtual Res<usize> write(Bytes) = 0;

    virtual Res<usize> seek(Io::Seek seek) = 0;

    virtual Res<usize> flush() = 0;

    virtual Res<Strong<Fd>> dup() = 0;

    virtual Res<Cons<Strong<Fd>, SocketAddr>> accept() = 0;

    virtual Res<Stat> stat() = 0;
};

struct DummyFd : public Fd {
    Res<usize> read(MutBytes) override;

    Res<usize> write(Bytes) override;

    Res<usize> seek(Io::Seek) override;

    Res<usize> flush() override;

    Res<Strong<Fd>> dup() override;

    Res<Cons<Strong<Fd>, SocketAddr>> accept() override;

    Res<Stat> stat() override;
};

template <typename T>
concept AsFd = requires(T t) {
    { t.underlying() } -> Meta::Same<Strong<Fd>>;
};

} // namespace Karm::Sys
