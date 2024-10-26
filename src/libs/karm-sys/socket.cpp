#include "socket.h"

#include "_embed.h"

namespace Karm::Sys {

// MARK: Udp Socket ------------------------------------------------------------

Res<UdpConnection> UdpConnection::listen(SocketAddr addr) {
    auto fd = try$(_Embed::listenUdp(addr));
    return Ok(UdpConnection(std::move(fd), addr));
}

// MARK: Tcp Socket ------------------------------------------------------------

Res<TcpConnection> TcpConnection::connect(SocketAddr addr) {
    auto fd = try$(_Embed::connectTcp(addr));
    return Ok(TcpConnection(std::move(fd), addr));
}

Res<TcpListener> TcpListener::listen(SocketAddr addr) {
    auto fd = try$(_Embed::listenTcp(addr));
    return Ok(TcpListener(std::move(fd), addr));
}

// MARK: Ipc Socket ------------------------------------------------------------

Res<IpcListener> IpcListener::listen(Mime::Url url) {
    auto fd = try$(_Embed::listenIpc(url));
    return Ok(IpcListener(std::move(fd), url));
}

} // namespace Karm::Sys
