#pragma once

#include <karm-base/rc.h>
#include <karm-io/pack.h>
#include <karm-io/types.h>
#include <karm-meta/nocopy.h>
#include <karm-meta/traits.h>
#include <karm-mime/url.h>

#include "addr.h"
#include "stat.h"
#include "types.h"

namespace Karm::Sys {

struct Fd;

using _Accepted = Pair<Rc<Fd>, SocketAddr>;
using _Sent = Pair<usize, usize>;
using _Received = Tuple<usize, usize, SocketAddr>;

struct Fd : Meta::NoCopy {
    virtual ~Fd() = default;

    virtual Handle handle() const = 0;

    virtual Res<usize> read(MutBytes) = 0;

    virtual Res<usize> write(Bytes) = 0;

    virtual Res<usize> seek(Io::Seek) = 0;

    virtual Res<> flush() = 0;

    virtual Res<Rc<Fd>> dup() = 0;

    virtual Res<_Accepted> accept() = 0;

    virtual Res<Stat> stat() = 0;

    virtual Res<_Sent> send(Bytes, Slice<Handle>, SocketAddr) = 0;

    virtual Res<_Received> recv(MutBytes, MutSlice<Handle>) = 0;

    virtual Res<> pack(Io::PackEmit& e) = 0;

    static Res<Rc<Fd>> unpack(Io::PackScan& s);
};

struct NullFd : Fd {
    Handle handle() const override {
        return INVALID;
    }

    Res<usize> read(MutBytes) override;

    Res<usize> write(Bytes) override;

    Res<usize> seek(Io::Seek) override;

    Res<> flush() override;

    Res<Rc<Fd>> dup() override;

    Res<_Accepted> accept() override;

    Res<Stat> stat() override;

    Res<_Sent> send(Bytes, Slice<Handle>, SocketAddr) override;

    Res<_Received> recv(MutBytes, MutSlice<Handle>) override;

    Res<> pack(Io::PackEmit& e) override;
};

template <typename T>
concept AsFd = requires(T t) {
    { t.fd() } -> Meta::Same<Rc<Fd>>;
};

} // namespace Karm::Sys
