#include "socket.h"

#include "_embed.h"

namespace Karm::Sys {

/* --- Tcp Socket ----------------------------------------------------------- */

Res<TcpStream> TcpStream::connect(SocketAddr addr) {
    auto fd = try$(_Embed::connectTcp(addr));
    return Ok(TcpStream(std::move(fd), addr));
}

Res<TcpListener> TcpListener::listen(SocketAddr addr) {
    auto fd = try$(_Embed::listenTcp(addr));
    return Ok(TcpListener(std::move(fd), addr));
}

Res<TcpStream> TcpListener::accept() {
    auto [fd, addr] = try$(_fd->accept());
    return Ok(TcpStream(std::move(fd), addr));
}

} // namespace Karm::Sys
