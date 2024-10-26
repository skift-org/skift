#pragma once

#include "addr.h"
#include "async.h"
#include "fd.h"

namespace Karm::Sys {

// MARK: Abstract Socket -------------------------------------------------------

struct _Connection :
    public Io::Reader,
    public Io::Writer,
    public Io::Flusher,
    Meta::NoCopy {

    virtual Async::Task<usize> readAsync(MutBytes buf) = 0;

    virtual Async::Task<usize> writeAsync(Bytes buf) = 0;

    virtual Async::Task<usize> flushAsync() = 0;
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

    Async::Task<usize> readAsync(MutBytes buf) override {
        return globalSched().readAsync(_fd, buf);
    }

    Res<usize> write(Bytes buf) override {
        return _fd->write(buf);
    }

    Async::Task<usize> writeAsync(Bytes buf) override {
        return globalSched().writeAsync(_fd, buf);
    }

    Res<usize> flush() override {
        return _fd->flush();
    }

    Async::Task<usize> flushAsync() override {
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

    Async::Task<C> acceptAsync() {
        auto [fd, addr] = co_trya$(globalSched().acceptAsync(_fd));
        co_return Ok(C(std::move(fd), addr));
    }

    Strong<Fd> fd() { return _fd; }
};

// MARK: Udp Socket ------------------------------------------------------------

struct UdpConnection :
    Meta::NoCopy {

    Strong<Sys::Fd> _fd;
    SocketAddr _addr;

    static Res<UdpConnection> listen(SocketAddr addr);

    UdpConnection(Strong<Sys::Fd> fd, SocketAddr addr)
        : _fd(std::move(fd)), _addr(addr) {}

    Res<usize> send(Bytes buf, SocketAddr addr) {
        auto [nbytes, _] = try$(_fd->send(buf, {}, addr));
        return Ok(nbytes);
    }

    auto sendAsync(Bytes buf, SocketAddr addr) {
        return globalSched().sendAsync(_fd, buf, {}, addr);
    }

    Res<Cons<usize, SocketAddr>> recv(MutBytes buf) {
        auto [nbytes, _, addr] = try$(_fd->recv(buf, {}));
        return Ok<Cons<usize, SocketAddr>>(nbytes, addr);
    }

    auto recvAsync(MutBytes buf) {
        return globalSched().recvAsync(_fd, buf, {});
    }
};

// MARK: Tcp Socket ------------------------------------------------------------

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

// MARK: Ipc Socket -----------------------------------------------------------

struct IpcConnection {
    Strong<Sys::Fd> _fd;
    Opt<Mime::Url> _url;

    static constexpr usize MAX_BUF_SIZE = 4096;
    static constexpr usize MAX_HND_SIZE = 16;

    static Res<IpcConnection> connect(Mime::Url url);

    IpcConnection(Strong<Sys::Fd> fd, Opt<Mime::Url> url)
        : _fd(std::move(fd)), _url(std::move(url)) {}

    Res<> send(Bytes buf, Slice<Handle> hnds) {
        try$(_fd->send(buf, hnds, {Ip4::unspecified(), 0}));
        return Ok();
    }

    Res<Cons<usize>> recv(MutBytes buf, MutSlice<Handle> hnds) {
        auto [nbytes, nhnds, _] = try$(_fd->recv(buf, hnds));
        return Ok<Cons<usize>>(nbytes, nhnds);
    }

    Async::Task<> sendAsync(Bytes buf, Slice<Handle> hnds) {
        co_trya$(globalSched().sendAsync(_fd, buf, hnds, Ip4::unspecified(0)));
        co_return Ok();
    }

    Async::Task<Cons<usize>> recvAsync(MutBytes buf, MutSlice<Handle> hnds) {
        auto [nbytes, nhnds, _] = co_trya$(globalSched().recvAsync(_fd, buf, hnds));
        co_return Ok<Cons<usize>>(nbytes, nhnds);
    }
};

struct IpcListener {
    Strong<Fd> _fd;
    Opt<Mime::Url> _url;

    static Res<IpcListener> listen(Mime::Url url);

    IpcListener(Strong<Sys::Fd> fd, Mime::Url url)
        : _fd(fd), _url(url) {}

    Res<IpcConnection> accept() {
        auto [fd, _] = try$(_fd->accept());
        return Ok(IpcConnection(std::move(fd), NONE));
    }

    Async::Task<IpcConnection> acceptAsync() {
        auto [fd, _] = co_trya$(globalSched().acceptAsync(_fd));
        co_return Ok(IpcConnection(std::move(fd), NONE));
    }

    Strong<Fd> fd() { return _fd; }
};

} // namespace Karm::Sys
