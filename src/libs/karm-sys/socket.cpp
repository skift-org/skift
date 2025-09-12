#include "socket.h"

#include "_embed.h"
#include "proc.h"

namespace Karm::Sys {

// MARK: Udp Socket ------------------------------------------------------------

Res<UdpConnection> UdpConnection::listen(SocketAddr addr) {
    try$(ensureUnrestricted());
    auto fd = try$(_Embed::listenUdp(addr));
    return Ok(UdpConnection(std::move(fd), addr));
}

// MARK: Tcp Socket ------------------------------------------------------------

Res<TcpConnection> TcpConnection::connect(SocketAddr addr) {
    try$(ensureUnrestricted());
    auto fd = try$(_Embed::connectTcp(addr));
    return Ok(TcpConnection(std::move(fd), addr));
}

Res<TcpListener> TcpListener::listen(SocketAddr addr) {
    try$(ensureUnrestricted());
    auto fd = try$(_Embed::listenTcp(addr));
    return Ok(TcpListener(std::move(fd), addr));
}

// MARK: Ipc Socket ------------------------------------------------------------

Res<IpcListener> IpcListener::listen(Ref::Url url) {
    try$(ensureUnrestricted());
    auto fd = try$(_Embed::listenIpc(url));
    return Ok(IpcListener(std::move(fd), url));
}

} // namespace Karm::Sys
