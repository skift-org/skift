#pragma once

#include "addr.h"
#include "async.h"
#include "fd.h"

namespace Karm::Sys {

// MARK: Abstract Socket -------------------------------------------------------

struct _Connection :
    Io::Reader,
    Io::Writer,
    Io::Flusher,
    Meta::NoCopy {

    virtual Async::Task<usize> readAsync(MutBytes buf) = 0;

    virtual Async::Task<usize> writeAsync(Bytes buf) = 0;

    virtual Async::Task<> flushAsync() = 0;
};

struct Connection :
    _Connection {

    Rc<Sys::Fd> _fd;

    Connection(Rc<Sys::Fd> fd)
        : _fd(std::move(fd)) {}

    Connection(Connection&&) = default;

    Connection& operator=(Connection&&) = default;

    Res<usize> read(MutBytes buf) override {
        return _fd->read(buf);
    }

    [[clang::coro_wrapper]]
    Async::Task<usize> readAsync(MutBytes buf) override {
        return globalSched().readAsync(_fd, buf);
    }

    Res<usize> write(Bytes buf) override {
        return _fd->write(buf);
    }

    [[clang::coro_wrapper]]
    Async::Task<usize> writeAsync(Bytes buf) override {
        return globalSched().writeAsync(_fd, buf);
    }

    Res<> flush() override {
        return _fd->flush();
    }

    [[clang::coro_wrapper]]
    Async::Task<> flushAsync() override {
        return globalSched().flushAsync(_fd);
    }

    Rc<Fd> fd() { return _fd; }
};

template <typename C>
struct _Listener :
    Meta::NoCopy {

    Rc<Sys::Fd> _fd;

    _Listener(Rc<Sys::Fd> fd)
        : _fd(std::move(fd)) {}

    Res<C> accept() {
        auto [fd, addr] = try$(_fd->accept());
        return Ok(C(std::move(fd), addr));
    }

    Async::Task<C> acceptAsync() {
        auto [fd, addr] = co_trya$(globalSched().acceptAsync(_fd));
        co_return Ok(C(std::move(fd), addr));
    }

    Rc<Fd> fd() { return _fd; }
};

// MARK: Udp Socket ------------------------------------------------------------

struct UdpConnection :
    Meta::NoCopy {

    Rc<Sys::Fd> _fd;
    SocketAddr _addr;

    static Res<UdpConnection> listen(SocketAddr addr);

    UdpConnection(Rc<Sys::Fd> fd, SocketAddr addr)
        : _fd(std::move(fd)), _addr(addr) {}

    Res<usize> send(Bytes buf, SocketAddr addr) {
        auto [nbytes, _] = try$(_fd->send(buf, {}, addr));
        return Ok(nbytes);
    }

    [[clang::coro_wrapper]]
    auto sendAsync(Bytes buf, SocketAddr addr) {
        return globalSched().sendAsync(_fd, buf, {}, addr);
    }

    Res<Pair<usize, SocketAddr>> recv(MutBytes buf) {
        auto [nbytes, _, addr] = try$(_fd->recv(buf, {}));
        return Ok<Pair<usize, SocketAddr>>(nbytes, addr);
    }

    [[clang::coro_wrapper]]
    auto recvAsync(MutBytes buf) {
        return globalSched().recvAsync(_fd, buf, {});
    }
};

// MARK: Tcp Socket ------------------------------------------------------------

struct TcpConnection :
    Connection {

    SocketAddr _addr;

    static Res<TcpConnection> connect(SocketAddr addr);

    TcpConnection(Rc<Sys::Fd> fd, SocketAddr addr)
        : Connection(std::move(fd)), _addr(addr) {}

    SocketAddr addr() const {
        return _addr;
    }
};

struct TcpListener :
    _Listener<TcpConnection> {

    SocketAddr _addr;

    static Res<TcpListener> listen(SocketAddr addr);

    TcpListener(Rc<Sys::Fd> fd, SocketAddr addr)
        : _Listener(std::move(fd)), _addr(addr) {}

    SocketAddr addr() const {
        return _addr;
    }
};

// MARK: Ipc Socket -----------------------------------------------------------

struct IpcConnection {
    Rc<Sys::Fd> _fd;
    Opt<Mime::Url> _url;

    static constexpr usize MAX_BUF_SIZE = 4096;
    static constexpr usize MAX_HND_SIZE = 16;

    static Res<IpcConnection> connect(Mime::Url url);

    IpcConnection(Rc<Sys::Fd> fd, Opt<Mime::Url> url)
        : _fd(std::move(fd)), _url(std::move(url)) {}

    Res<> send(Bytes buf, Slice<Handle> hnds) {
        try$(_fd->send(buf, hnds, {Ip4::unspecified(), 0}));
        return Ok();
    }

    Res<Pair<usize>> recv(MutBytes buf, MutSlice<Handle> hnds) {
        auto [nbytes, nhnds, _] = try$(_fd->recv(buf, hnds));
        return Ok<Pair<usize>>(nbytes, nhnds);
    }

    Async::Task<> sendAsync(Bytes buf, Slice<Handle> hnds) {
        co_trya$(globalSched().sendAsync(_fd, buf, hnds, Ip4::unspecified(0)));
        co_return Ok();
    }

    Async::Task<Pair<usize>> recvAsync(MutBytes buf, MutSlice<Handle> hnds) {
        auto [nbytes, nhnds, _] = co_trya$(globalSched().recvAsync(_fd, buf, hnds));
        co_return Ok<Pair<usize>>(nbytes, nhnds);
    }
};

struct IpcListener {
    Rc<Fd> _fd;
    Opt<Mime::Url> _url;

    static Res<IpcListener> listen(Mime::Url url);

    IpcListener(Rc<Sys::Fd> fd, Mime::Url url)
        : _fd(fd), _url(url) {}

    Res<IpcConnection> accept() {
        auto [fd, _] = try$(_fd->accept());
        return Ok(IpcConnection(std::move(fd), NONE));
    }

    Async::Task<IpcConnection> acceptAsync() {
        auto [fd, _] = co_trya$(globalSched().acceptAsync(_fd));
        co_return Ok(IpcConnection(std::move(fd), NONE));
    }

    Rc<Fd> fd() { return _fd; }
};

} // namespace Karm::Sys
