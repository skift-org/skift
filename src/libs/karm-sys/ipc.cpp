#include "ipc.h"

namespace Karm::Sys {

Async::Task<Message> ipcRecvAsync(Sys::IpcConnection &con) {
    Message msg;
    auto [bufLen, hndsLen] = co_trya$(con.recvAsync(msg.bytes, msg.handles));
    msg.len = bufLen;
    msg.handlesLen = hndsLen;
    co_return msg;
}

} // namespace Karm::Sys
