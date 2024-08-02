#include <errno.h>

#include "utils.h"

namespace Posix {

Error fromErrno(isize error) {
    if (EOPNOTSUPP != ENOTSUP and error == EOPNOTSUPP) {
        return Error::unsupported("operation not supported on socket");
    }

    if (EWOULDBLOCK != EAGAIN and error == EWOULDBLOCK) {
        return Error::wouldBlock("operation would block");
    }

    switch (error) {
    case 0:
        return Error::other("success");
    case E2BIG:
        return Error::argumentListTooLong("argument list too long");
    case EACCES:
        return Error::permissionDenied("permission denied");
    case EADDRINUSE:
        return Error::addrInUse("address in use");
    case EADDRNOTAVAIL:
        return Error::addrNotAvailable("address not available");
    case EAFNOSUPPORT:
        return Error::unsupported("address family not supported");
    case EAGAIN:
        return Error::resourceBusy("resource unavailable, try again");
    case EALREADY:
        return Error::resourceBusy("connection already in progress");
    case EBADF:
        return Error::invalidInput("bad file descriptor");
    case EBADMSG:
        return Error::invalidData("bad message");
    case EBUSY:
        return Error::resourceBusy("device or resource busy");
    case ECANCELED:
        return Error::interrupted("operation canceled");
    case ECHILD:
        return Error::other("no child processes");
    case ECONNABORTED:
        return Error::connectionAborted("connection aborted");
    case ECONNREFUSED:
        return Error::connectionRefused("connection refused");
    case ECONNRESET:
        return Error::connectionReset("connection reset");
    case EDEADLK:
        return Error::deadlock("resource deadlock would occur");
    case EDESTADDRREQ:
        return Error::invalidInput("destination address required");
    case EDOM:
        return Error::invalidInput("mathematics argument out of domain of function");
    case EEXIST:
        return Error::alreadyExists("file exists");
    case EFAULT:
        return Error::invalidInput("bad address");
    case EFBIG:
        return Error::fileTooLarge("file too large");
    case EHOSTUNREACH:
        return Error::hostUnreachable("host is unreachable");
    case EIDRM:
        return Error::invalidInput("identifier removed");
    case EILSEQ:
        return Error::invalidInput("illegal byte sequence");
    case EINPROGRESS:
        return Error::resourceBusy("operation in progress");
    case EINTR:
        return Error::interrupted("interrupted function");
    case EINVAL:
        return Error::invalidInput("invalid argument");
    case EIO:
        return Error::invalidInput("i/o error");
    case EISCONN:
        return Error::other("socket is connected");
    case EISDIR:
        return Error::isADirectory("is a directory");
    case ELOOP:
        return Error::filesystemLoop("too many levels of symbolic links");
    case EMFILE:
        return Error::invalidInput("file descriptor value too large");
    case EMLINK:
        return Error::tooManyLinks("too many links");
    case EMSGSIZE:
        return Error::invalidInput("message too large");
    case ENAMETOOLONG:
        return Error::invalidInput("filename too long");
    case ENETDOWN:
        return Error::networkDown("network is down");
    case ENETRESET:
        return Error::connectionReset("connection aborted by network");
    case ENETUNREACH:
        return Error::networkUnreachable("network unreachable");
    case ENFILE:
        return Error::limitReached("too many files open in system");
    case ENOBUFS:
        return Error::limitReached("no buffer space available");
    case ENODATA:
        return Error::other("no message is available on the STREAM head read queue");
    case ENODEV:
        return Error::notFound("no such device");
    case ENOENT:
        return Error::notFound("no such file or directory");
    case ENOEXEC:
        return Error::invalidData("executable file format error");
    case ENOLCK:
        return Error::limitReached("no locks available");
    case ENOMEM:
        return Error::outOfMemory("not enough space");
    case ENOMSG:
        return Error::other("no message of the desired type");
    case ENOPROTOOPT:
        return Error::invalidInput("protocol not available");
    case ENOSPC:
        return Error::storageFull("no space left on device");
    case ENOSR:
        return Error::limitReached("no STREAM resources");
    case ENOSTR:
        return Error::invalidInput("not a STREAM");
    case ENOSYS:
        return Error::unsupported("functionality not supported");
    case ENOTCONN:
        return Error::notConnected("the socket is not connected");
    case ENOTDIR:
        return Error::invalidInput("not a directory");
    case ENOTEMPTY:
        return Error::directoryNotEmpty("directory not empty");
    case ENOTRECOVERABLE:
        return Error::other("state not recoverable");
    case ENOTSOCK:
        return Error::invalidInput("not a socket");
    case ENOTSUP:
        return Error::unsupported("operation not supported");
    case ENOTTY:
        return Error::invalidInput("inappropriate i/o control operation");
    case ENXIO:
        return Error::notFound("no such device or address");
    case EOVERFLOW:
        return Error::other("value too large to be stored in data type");
    case EOWNERDEAD:
        return Error::other("previous owner died");
    case EPERM:
        return Error::permissionDenied("operation not permitted");
    case EPIPE:
        return Error::brokenPipe("broken pipe");
    case EPROTO:
        return Error::other("protocol error");
    case EPROTONOSUPPORT:
        return Error::unsupported("protocol not supported");
    case EPROTOTYPE:
        return Error::invalidInput("protocol wrong type for socket");
    case ERANGE:
        return Error::invalidInput("result too large");
    case EROFS:
        return Error::readOnlyFilesystem("read-only file system");
    case ESPIPE:
        return Error::invalidInput("invalid seek");
    case ESRCH:
        return Error::notFound("no such process");
    case ETIME:
        return Error::timedOut("stream ioctl() timeout");
    case ETIMEDOUT:
        return Error::timedOut("connection timed out");
    case ETXTBSY:
        return Error::resourceBusy("text file busy");
    case EXDEV:
        return Error::invalidInput("cross-device link");
    default:
        return Error::other("unknown errno");
    }
}

Error fromLastErrno() {
    return fromErrno(errno);
}

// Construct an error from a process status code.
Error fromStatus(isize status) {
    if (WIFEXITED(status)) {
        return fromErrno(WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        return Error::other("process terminated by signal");
    } else {
        return Error::other("process terminated abnormally");
    }
}

Res<> consumeErrno() {
    if (errno == 0) {
        return Ok();
    }

    auto err = fromLastErrno();
    errno = 0;
    return err;
}

struct sockaddr_in toSockAddr(Sys::SocketAddr addr) {
    struct sockaddr_in sockaddr;
    auto addr4 = addr.addr.unwrap<Sys::Ip4>();
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(addr.port);
    sockaddr.sin_addr.s_addr = addr4._raw._value;
    return sockaddr;
}

Sys::SocketAddr fromSockAddr(struct sockaddr_in sockaddr) {
    Sys::SocketAddr addr{Sys::Ip4::unspecified(), 0};
    addr.addr.unwrap<Sys::Ip4>()._raw._value = sockaddr.sin_addr.s_addr;
    addr.port = ntohs(sockaddr.sin_port);
    return addr;
}

Sys::Stat fromStat(struct stat const &buf) {
    Sys::Stat stat{};
    Sys::Stat::Type type = Sys::Stat::FILE;
    if (S_ISDIR(buf.st_mode))
        type = Sys::Stat::DIR;
    stat.type = type;
    stat.size = (usize)buf.st_size;
    stat.accessTime = TimeStamp::epoch() + TimeSpan::fromSecs(buf.st_atime);
    stat.modifyTime = TimeStamp::epoch() + TimeSpan::fromSecs(buf.st_mtime);
    stat.changeTime = TimeStamp::epoch() + TimeSpan::fromSecs(buf.st_ctime);
    return stat;
}

struct timespec toTimespec(TimeStamp ts) {
    struct timespec pts;
    if (ts.isEndOfTime()) {
        pts.tv_sec = Limits<long>::MAX;
        pts.tv_nsec = 0;
        return pts;
    } else {
        pts.tv_sec = ts.val() / 1000000;
        pts.tv_nsec = (ts.val() % 1000000) * 1000;
    }
    return pts;
}

struct timespec toTimespec(TimeSpan ts) {
    struct timespec pts;
    if (ts.isInfinite()) {
        pts.tv_sec = Limits<long>::MAX;
        pts.tv_nsec = 0;
        return pts;
    } else {
        pts.tv_sec = ts.val() / 1000000;
        pts.tv_nsec = (ts.val() % 1000000) * 1000;
    }
    return pts;
}

Res<Str> repoRoot() {
    auto *maybeRepo = getenv("CK_BUILDDIR");

    if (not maybeRepo)
        maybeRepo = getenv("SKIFT_BUNDLES");

    if (not maybeRepo)
        return Error::notFound("SKIFT_BUNDLES not set");

    return Ok(Str::fromNullterminated(maybeRepo));
}

} // namespace Posix
