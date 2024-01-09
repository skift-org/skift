#pragma once

#include <karm-sys/fd.h>

namespace Posix {

struct Fd : public Sys::Fd {
    isize _raw;

    Fd(isize raw);

    ~Fd() override;

    usize handle() const override;

    Res<usize> read(MutBytes bytes) override;

    Res<usize> write(Bytes bytes) override;

    Res<usize> seek(Io::Seek seek) override;

    Res<usize> flush() override;

    Res<Strong<Sys::Fd>> dup() override;

    Res<Sys::Accepted> accept() override;

    Res<Sys::Stat> stat() override;

    Res<> sendFd(Strong<Sys::Fd> fd) override;

    Res<Strong<Sys::Fd>> recvFd() override;

    Res<usize> sendTo(Bytes bytes, Sys::SocketAddr addr) override;

    Res<Cons<usize, Sys::SocketAddr>> recvFrom(MutBytes bytes) override;
};

} // namespace Posix
