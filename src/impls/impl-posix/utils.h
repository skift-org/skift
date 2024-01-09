#pragma once

#include <netinet/in.h>
#include <sys/stat.h>

//
#include <karm-base/error.h>
#include <karm-base/res.h>
#include <karm-sys/fd.h>

namespace Posix {

Error fromErrno(isize error);

Error fromLastErrno();

Res<> consumeErrno();

struct sockaddr_in toSockAddr(Sys::SocketAddr addr);

Sys::SocketAddr fromSockAddr(struct sockaddr_in sockaddr);

Sys::Stat fromStat(struct stat const &buf);

} // namespace Posix
