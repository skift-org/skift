#pragma once

#include <karm-sys/fd.h>

namespace Karm::Posix {

struct Fd : Sys::Fd {
    isize _raw;
    bool _leak = false; //< Do not close on destruction

    Fd(isize raw);

    ~Fd() override;

    Sys::Handle handle() const override;

    Res<usize> read(MutBytes bytes) override;

    Res<usize> write(Bytes bytes) override;

    Res<usize> seek(Io::Seek seek) override;

    Res<> flush() override;

    Res<Rc<Sys::Fd>> dup() override;

    Res<Sys::_Accepted> accept() override;

    Res<Sys::Stat> stat() override;

    Res<Sys::_Sent> send(Bytes, Slice<Sys::Handle>, Sys::SocketAddr) override;

    Res<Sys::_Received> recv(MutBytes, MutSlice<Sys::Handle>) override;

    Res<> pack(Sys::MessageWriter& e) override;
};

} // namespace Karm::Posix
