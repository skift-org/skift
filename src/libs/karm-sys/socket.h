#pragma once

#include "addr.h"
#include "async.h"
#include "fd.h"

namespace Karm::Sys {

/* --- Abstract Socket ------------------------------------------------------ */

struct _Connection :
    public Io::Reader,
    public Io::Writer,
    public Io::Flusher,
    Meta::NoCopy {

    virtual Task<usize> readAsync(MutBytes buf) = 0;

    virtual Task<usize> writeAsync(Bytes buf) = 0;

    virtual Task<usize> flushAsync() = 0;
};

struct Connection :
    public _Connection {

    Strong<Sys::Fd> _fd;

    Connection(Strong<Sys::Fd> fd)
        : _fd(std::move(fd)) {}

    Connection(Connection &&) = default;

    Connection &operator=(Connection &&) = default;

    Res<usize> read(MutBytes buf) override {
        return _fd->read(buf);
    }

    Task<usize> readAsync(MutBytes buf) override {
        return globalSched().readAsync(_fd, buf);
    }

    Res<usize> write(Bytes buf) override {
        return _fd->write(buf);
    }

    Task<usize> writeAsync(Bytes buf) override {
        return globalSched().writeAsync(_fd, buf);
    }

    Res<usize> flush() override {
        return _fd->flush();
    }

    Task<usize> flushAsync() override {
        return globalSched().flushAsync(_fd);
    }

    Strong<Fd> fd() { return _fd; }
};

template <typename C>
struct _Listener :
    Meta::NoCopy {

    Strong<Sys::Fd> _fd;

    _Listener(Strong<Sys::Fd> fd)
        : _fd(std::move(fd)) {}

    Res<C> accept() {
        auto [fd, addr] = try$(_fd->accept());
        return Ok(C(std::move(fd), addr));
    }

    Task<C> acceptAsync() {
        auto [fd, addr] = co_try_await$(globalSched().acceptAsync(_fd));
        co_return Ok(C(std::move(fd), addr));
    }

    Strong<Fd> fd() { return _fd; }
};

/* --- Udp Socket ----------------------------------------------------------- */

struct UdpConnection :
    Meta::NoCopy {

    SocketAddr _addr;
    Strong<Sys::Fd> _fd;

    static Res<UdpConnection> listen(SocketAddr addr);

    UdpConnection(Strong<Sys::Fd> fd, SocketAddr addr)
        : _addr(addr), _fd(std::move(fd)) {}

    Res<usize> send(Bytes buf, SocketAddr addr) {
        return _fd->sendTo(buf, addr);
    }

    auto sendAsync(Bytes buf, SocketAddr addr) {
        return globalSched().sendAsync(_fd, buf, addr);
    }

    Res<Received> recv(MutBytes buf) {
        return _fd->recvFrom(buf);
    }

    auto recvAsync(MutBytes buf) {
        return globalSched().recvAsync(_fd, buf);
    }
};

/* --- Tcp Socket ----------------------------------------------------------- */

struct TcpConnection :
    public Connection {

    SocketAddr _addr;

    static Res<TcpConnection> connect(SocketAddr addr);

    TcpConnection(Strong<Sys::Fd> fd, SocketAddr addr)
        : Connection(std::move(fd)), _addr(addr) {}

    SocketAddr addr() const {
        return _addr;
    }
};

struct TcpListener :
    public _Listener<TcpConnection> {

    SocketAddr _addr;

    static Res<TcpListener> listen(SocketAddr addr);

    TcpListener(Strong<Sys::Fd> fd, SocketAddr addr)
        : _Listener(std::move(fd)), _addr(addr) {}

    SocketAddr addr() const {
        return _addr;
    }
};

/* --- Ipc Socket ---------------------------------------------------------- */

struct IpcConnection :
    public Connection {

    Opt<Url::Url> _url;

    static Res<IpcConnection> connect(Url::Url url);

    IpcConnection(Strong<Sys::Fd> fd, Url::Url url)
        : Connection(std::move(fd)), _url(std::move(url)) {}

    Res<> sendFd(AsFd auto &fd) {
        return _fd->sendFd(fd.fd());
    }

    Res<Strong<Fd>> recvFd() {
        return _fd->recvFd();
    }
};

struct IpcListener :
    public _Listener<TcpConnection> {

    Opt<Url::Url> _url;

    static Res<IpcListener> listen(Url::Url url);

    IpcListener(Strong<Sys::Fd> fd, Url::Url url)
        : _Listener(std::move(fd)), _url(std::move(url)) {}
};

} // namespace Karm::Sys
