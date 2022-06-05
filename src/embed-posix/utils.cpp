#include <errno.h>

#include "utils.h"

namespace Embed::Posix {

Error fromErrno(int error) {
    if (EOPNOTSUPP != ENOTSUP && error == EOPNOTSUPP) {
        return Error(Error::UNSUPPORTED, "operation not supported on socket");
    }

    if (EWOULDBLOCK != EAGAIN && error == EWOULDBLOCK) {
        return Error(Error::WOULD_BLOCK, "operation would block");
    }

    switch (error) {
    case E2BIG:
        return Error(Error::ARGUMENT_LIST_TOO_LONG, "argument list too long");
    case EACCES:
        return Error(Error::PERMISSION_DENIED, "permission denied");
    case EADDRINUSE:
        return Error(Error::ADDR_IN_USE, "address in use");
    case EADDRNOTAVAIL:
        return Error(Error::ADDR_NOT_AVAILABLE, "address not available");
    case EAFNOSUPPORT:
        return Error(Error::UNSUPPORTED, "address family not supported");
    case EAGAIN:
        return Error(Error::RESOURCE_BUSY, "resource unavailable, try again");
    case EALREADY:
        return Error(Error::RESOURCE_BUSY, "connection already in progress");
    case EBADF:
        return Error(Error::INVALID_INPUT, "bad file descriptor");
    case EBADMSG:
        return Error(Error::INVALID_DATA, "bad message");
    case EBUSY:
        return Error(Error::RESOURCE_BUSY, "device or resource busy");
    case ECANCELED:
        return Error(Error::OTHER, "operation canceled");
    case ECHILD:
        return Error(Error::OTHER, "no child processes");
    case ECONNABORTED:
        return Error(Error::CONNECTION_ABORTED, "connection aborted");
    case ECONNREFUSED:
        return Error(Error::CONNECTION_REFUSED, "connection refused");
    case ECONNRESET:
        return Error(Error::CONNECTION_RESET, "connection reset");
    case EDEADLK:
        return Error(Error::DEADLOCK, "resource deadlock would occur");
    case EDESTADDRREQ:
        return Error(Error::INVALID_INPUT, "destination address required");
    case EDOM:
        return Error(Error::INVALID_INPUT, "mathematics argument out of domain of function");
    case EEXIST:
        return Error(Error::ALREADY_EXISTS, "file exists");
    case EFAULT:
        return Error(Error::INVALID_INPUT, "bad address");
    case EFBIG:
        return Error(Error::FILE_TOO_LARGE, "file too large");
    case EHOSTUNREACH:
        return Error(Error::HOST_UNREACHABLE, "host is unreachable");
    case EIDRM:
        return Error(Error::INVALID_INPUT, "identifier removed");
    case EILSEQ:
        return Error(Error::INVALID_INPUT, "illegal byte sequence");
    case EINPROGRESS:
        return Error(Error::RESOURCE_BUSY, "operation in progress");
    case EINTR:
        return Error(Error::INTERRUPTED, "interrupted function");
    case EINVAL:
        return Error(Error::INVALID_INPUT, "invalid argument");
    case EIO:
        return Error(Error::INVALID_INPUT, "i/o error");
    case EISCONN:
        return Error(Error::OTHER, "socket is connected");
    case EISDIR:
        return Error(Error::IS_A_DIRECTORY, "is a directory");
    case ELOOP:
        return Error(Error::FILESYSTEM_LOOP, "too many levels of symbolic links");
    case EMFILE:
        return Error(Error::INVALID_INPUT, "file descriptor value too large");
    case EMLINK:
        return Error(Error::TOO_MANY_LINKS, "too many links");
    case EMSGSIZE:
        return Error(Error::INVALID_INPUT, "message too large");
    case ENAMETOOLONG:
        return Error(Error::INVALID_INPUT, "filename too long");
    case ENETDOWN:
        return Error(Error::NETWORK_DOWN, "network is down");
    case ENETRESET:
        return Error(Error::CONNECTION_RESET, "connection aborted by network");
    case ENETUNREACH:
        return Error(Error::NETWORK_UNREACHABLE, "network unreachable");
    case ENFILE:
        return Error(Error::LIMIT_REACHED, "too many files open in system");
    case ENOBUFS:
        return Error(Error::LIMIT_REACHED, "no buffer space available");
    case ENODATA:
        return Error(Error::OTHER, "no message is available on the STREAM head read queue");
    case ENODEV:
        return Error(Error::NOT_FOUND, "no such device");
    case ENOENT:
        return Error(Error::NOT_FOUND, "no such file or directory");
    case ENOEXEC:
        return Error(Error::INVALID_DATA, "executable file format error");
    case ENOLCK:
        return Error(Error::LIMIT_REACHED, "no locks available");
    case ENOMEM:
        return Error(Error::OUT_OF_MEMORY, "not enough space");
    case ENOMSG:
        return Error(Error::OTHER, "no message of the desired type");
    case ENOPROTOOPT:
        return Error(Error::INVALID_INPUT, "protocol not available");
    case ENOSPC:
        return Error(Error::STORAGE_FULL, "no space left on device");
    case ENOSR:
        return Error(Error::LIMIT_REACHED, "no STREAM resources");
    case ENOSTR:
        return Error(Error::INVALID_INPUT, "not a STREAM");
    case ENOSYS:
        return Error(Error::UNSUPPORTED, "functionality not supported");
    case ENOTCONN:
        return Error(Error::NOT_CONNECTED, "the socket is not connected");
    case ENOTDIR:
        return Error(Error::INVALID_INPUT, "not a directory or a symbolic link to a directory");
    case ENOTEMPTY:
        return Error(Error::DIRECTORY_NOT_EMPTY, "directory not empty");
    case ENOTRECOVERABLE:
        return Error(Error::OTHER, "state not recoverable");
    case ENOTSOCK:
        return Error(Error::INVALID_INPUT, "not a socket");
    case ENOTSUP:
        return Error(Error::UNSUPPORTED, "not supported");
    case ENOTTY:
        return Error(Error::INVALID_INPUT, "inappropriate i/o control operation");
    case ENXIO:
        return Error(Error::NOT_FOUND, "no such device or address");
    case EOVERFLOW:
        return Error(Error::OTHER, "value too large to be stored in data type");
    case EOWNERDEAD:
        return Error(Error::OTHER, "previous owner died");
    case EPERM:
        return Error(Error::PERMISSION_DENIED, "operation not permitted");
    case EPIPE:
        return Error(Error::INVALID_DATA, "broken pipe");
    case EPROTO:
        return Error(Error::OTHER, "protocol error");
    case EPROTONOSUPPORT:
        return Error(Error::UNSUPPORTED, "protocol not supported");
    case EPROTOTYPE:
        return Error(Error::INVALID_INPUT, "protocol wrong type for socket");
    case ERANGE:
        return Error(Error::INVALID_INPUT, "result too large");
    case EROFS:
        return Error(Error::READ_ONLY_FILESYSTEM, "read-only file system");
    case ESPIPE:
        return Error(Error::INVALID_INPUT, "invalid seek");
    case ESRCH:
        return Error(Error::NOT_FOUND, "no such process");
    case ETIME:
        return Error(Error::TIMED_OUT, "stream ioctl() timeout");
    case ETIMEDOUT:
        return Error(Error::TIMED_OUT, "connection timed out");
    case ETXTBSY:
        return Error(Error::RESOURCE_BUSY, "text file busy");
    case EXDEV:
        return Error(Error::INVALID_INPUT, "cross-device link");
    default:
        return Error::OTHER;
    }
}

Error fromLastErrno() {
    return fromErrno(errno);
}

} // namespace Embed::Posix
