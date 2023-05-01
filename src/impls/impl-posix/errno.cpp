#include <errno.h>

#include "errno.h"

namespace Embed::Posix {

Error fromErrno(isize error) {
    if (EOPNOTSUPP != ENOTSUP and error == EOPNOTSUPP) {
        return Error::unsupported("operation not supported on socket");
    }

    if (EWOULDBLOCK != EAGAIN and error == EWOULDBLOCK) {
        return Error::wouldBlock("operation would block");
    }

    switch (error) {
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
        return Error::OTHER;
    }
}

Error fromLastErrno() {
    return fromErrno(errno);
}

Res<> consumeErrno() {
    if (errno == 0) {
        return Ok();
    } else {
        auto err = fromLastErrno();
        errno = 0;
        return err;
    }
}

} // namespace Embed::Posix
