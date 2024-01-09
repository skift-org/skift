#pragma once

#include <karm-base/rc.h>
#include <karm-io/types.h>
#include <karm-meta/nocopy.h>
#include <karm-meta/traits.h>
#include <url/url.h>

#include "addr.h"
#include "stat.h"

namespace Karm::Sys {

struct Fd;

using Accepted = Cons<Strong<Fd>, SocketAddr>;
using Received = Cons<usize, SocketAddr>;

struct Fd : Meta::NoCopy {
    virtual ~Fd() = default;

    virtual usize handle() const = 0;

    virtual Res<usize> read(MutBytes) = 0;

    virtual Res<usize> write(Bytes) = 0;

    virtual Res<usize> seek(Io::Seek seek) = 0;

    virtual Res<usize> flush() = 0;

    virtual Res<Strong<Fd>> dup() = 0;

    virtual Res<Accepted> accept() = 0;

    virtual Res<Stat> stat() = 0;

    virtual Res<> sendFd(Strong<Fd>) = 0;

    virtual Res<Strong<Fd>> recvFd() = 0;

    virtual Res<usize> sendTo(Bytes, SocketAddr) = 0;

    virtual Res<Received> recvFrom(MutBytes) = 0;
};

struct NullFd : public Fd {
    usize handle() const override { return -1; }

    Res<usize> read(MutBytes) override;

    Res<usize> write(Bytes) override;

    Res<usize> seek(Io::Seek) override;

    Res<usize> flush() override;

    Res<Strong<Fd>> dup() override;

    Res<Accepted> accept() override;

    Res<Stat> stat() override;

    Res<> sendFd(Strong<Fd>) override;

    Res<Strong<Fd>> recvFd() override;

    Res<usize> sendTo(Bytes, SocketAddr) override;

    Res<Received> recvFrom(MutBytes) override;
};

template <typename T>
concept AsFd = requires(T t) {
    { t.fd() } -> Meta::Same<Strong<Fd>>;
};

} // namespace Karm::Sys

