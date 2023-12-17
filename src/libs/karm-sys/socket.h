#pragma once


#include "addr.h"
#include "fd.h"

namespace Karm::Sys {

/* --- Tcp Socket ----------------------------------------------------------- */

struct TcpStream :
    public Io::Reader,
    public Io::Writer,
    public Meta::NoCopy {

    Strong<Sys::Fd> _fd;
    SocketAddr _addr;

    static Res<TcpStream> connect(SocketAddr addr);

    TcpStream(Strong<Sys::Fd> fd, SocketAddr addr)
        : _fd(std::move(fd)), _addr(addr) {}

    virtual Res<usize> read(MutBytes buf) override {
        return _fd->read(buf);
    }

    virtual Res<usize> write(Bytes buf) override {
        return _fd->write(buf);
    }
};

struct TcpListener :
    public Meta::NoCopy {

    Strong<Sys::Fd> _fd;
    SocketAddr _addr;

    static Res<TcpListener> listen(SocketAddr addr);

    TcpListener(Strong<Sys::Fd> fd, SocketAddr addr)
        : _fd(std::move(fd)), _addr(addr) {}

    Res<TcpStream> accept();
};

} // namespace Karm::Sys


