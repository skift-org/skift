#include <errno.h>

#include "utils.h"

namespace Karm::Embed::Posix {

Base::Error fromErrno(int error) {
    switch (error) {
    case E2BIG:
        return Base::Error(Base::Error::ARGUMENT_LIST_TOO_LONG, "Argument list too long.");
    case EACCES:
        return Base::Error(Base::Error::PERMISSION_DENIED, "Permission denied.");
    case EADDRINUSE:
        return Base::Error(Base::Error::ADDR_IN_USE, "Address in use.");
    case EADDRNOTAVAIL:
        return Base::Error(Base::Error::ADDR_NOT_AVAILABLE, "Address not available.");
    case EAFNOSUPPORT:
        return Base::Error(Base::Error::UNSUPPORTED, "Address family not supported.");
    case EAGAIN:
        return Base::Error(Base::Error::RESOURCE_BUSY, "Resource unavailable, try again.");
    case EALREADY:
        return Base::Error(Base::Error::RESOURCE_BUSY, "Connection already in progress.");
    case EBADF:
        return Base::Error(Base::Error::INVALID_INPUT, "Bad file descriptor.");
    case EBADMSG:
        return Base::Error(Base::Error::INVALID_DATA, "Bad message.");
    case EBUSY:
        return Base::Error(Base::Error::RESOURCE_BUSY, "Device or resource busy.");
    case ECANCELED:
        return Base::Error(Base::Error::OTHER, "Operation canceled.");
    case ECHILD:
        return Base::Error(Base::Error::OTHER, "No child processes.");
    case ECONNABORTED:
        return Base::Error(Base::Error::CONNECTION_ABORTED, "Connection aborted.");
    case ECONNREFUSED:
        return Base::Error(Base::Error::CONNECTION_REFUSED, "Connection refused.");
    case ECONNRESET:
        return Base::Error(Base::Error::CONNECTION_RESET, "Connection reset.");
    case EDEADLK:
        return Base::Error(Base::Error::DEADLOCK, "Resource deadlock would occur.");
    case EDESTADDRREQ:
        return Base::Error(Base::Error::INVALID_INPUT, "Destination address required.");
    case EDOM:
        return Base::Error(Base::Error::INVALID_INPUT, "Mathematics argument out of domain of function.");
    case EEXIST:
        return Base::Error(Base::Error::ALREADY_EXISTS, "File exists.");
    case EFAULT:
        return Base::Error(Base::Error::INVALID_INPUT, "Bad address.");
    case EFBIG:
        return Base::Error(Base::Error::FILE_TOO_LARGE, "File too large.");
    case EHOSTUNREACH:
        return Base::Error(Base::Error::HOST_UNREACHABLE, "Host is unreachable.");
    case EIDRM:
        return Base::Error(Base::Error::INVALID_INPUT, "Identifier removed.");
    case EILSEQ:
        return Base::Error(Base::Error::INVALID_INPUT, "Illegal byte sequence.");
    case EINPROGRESS:
        return Base::Error(Base::Error::RESOURCE_BUSY, "Operation in progress.");
    case EINTR:
        return Base::Error(Base::Error::INTERRUPTED, "Interrupted function.");
    case EINVAL:
        return Base::Error(Base::Error::INVALID_INPUT, "Invalid argument.");
    case EIO:
        return Base::Error(Base::Error::INVALID_INPUT, "I/O error.");
    case EISCONN:
        return Base::Error(Base::Error::OTHER, "Socket is connected.");
    case EISDIR:
        return Base::Error(Base::Error::IS_A_DIRECTORY, "Is a directory.");
    case ELOOP:
        return Base::Error(Base::Error::FILESYSTEM_LOOP, "Too many levels of symbolic links.");
    case EMFILE:
        return Base::Error(Base::Error::INVALID_INPUT, "File descriptor value too large.");
    case EMLINK:
        return Base::Error(Base::Error::TOO_MANY_LINKS, "Too many links.");
    case EMSGSIZE:
        return Base::Error(Base::Error::INVALID_INPUT, "Message too large.");
    case ENAMETOOLONG:
        return Base::Error(Base::Error::INVALID_INPUT, "Filename too long.");
    case ENETDOWN:
        return Base::Error(Base::Error::NETWORK_DOWN, "Network is down.");
    case ENETRESET:
        return Base::Error(Base::Error::CONNECTION_RESET, "Connection aborted by network.");
    case ENETUNREACH:
        return Base::Error(Base::Error::NETWORK_UNREACHABLE, "Network unreachable.");
    case ENFILE:
        return Base::Error(Base::Error::LIMIT_REACHED, "Too many files open in system.");
    case ENOBUFS:
        return Base::Error(Base::Error::LIMIT_REACHED, "No buffer space available.");
    case ENODATA:
        return Base::Error(Base::Error::OTHER, "No message is available on the STREAM head read queue.");
    case ENODEV:
        return Base::Error(Base::Error::NOT_FOUND, "No such device.");
    case ENOENT:
        return Base::Error(Base::Error::NOT_FOUND, "No such file or directory.");
    case ENOEXEC:
        return Base::Error(Base::Error::INVALID_DATA, "Executable file format error.");
    case ENOLCK:
        return Base::Error(Base::Error::LIMIT_REACHED, "No locks available.");
    case ENOMEM:
        return Base::Error(Base::Error::OUT_OF_MEMORY, "Not enough space.");
    case ENOMSG:
        return Base::Error(Base::Error::OTHER, "No message of the desired type.");
    case ENOPROTOOPT:
        return Base::Error(Base::Error::INVALID_INPUT, "Protocol not available.");
    case ENOSPC:
        return Base::Error(Base::Error::STORAGE_FULL, "No space left on device.");
    case ENOSR:
        return Base::Error(Base::Error::LIMIT_REACHED, "No STREAM resources.");
    case ENOSTR:
        return Base::Error(Base::Error::INVALID_INPUT, "Not a STREAM.");
    case ENOSYS:
        return Base::Error(Base::Error::UNSUPPORTED, "Functionality not supported.");
    case ENOTCONN:
        return Base::Error(Base::Error::NOT_CONNECTED, "The socket is not connected.");
    case ENOTDIR:
        return Base::Error(Base::Error::INVALID_INPUT, "Not a directory or a symbolic link to a directory.");
    case ENOTEMPTY:
        return Base::Error(Base::Error::DIRECTORY_NOT_EMPTY, "Directory not empty.");
    case ENOTRECOVERABLE:
        return Base::Error(Base::Error::OTHER, "State not recoverable.");
    case ENOTSOCK:
        return Base::Error(Base::Error::INVALID_INPUT, "Not a socket.");
    case ENOTSUP:
        return Base::Error(Base::Error::UNSUPPORTED, "Not supported.");
    case ENOTTY:
        return Base::Error(Base::Error::INVALID_INPUT, "Inappropriate I/O control operation.");
    case ENXIO:
        return Base::Error(Base::Error::NOT_FOUND, "No such device or address.");
    case EOPNOTSUPP:
        return Base::Error(Base::Error::UNSUPPORTED, "Operation not supported on socket.");
    case EOVERFLOW:
        return Base::Error(Base::Error::OTHER, "Value too large to be stored in data type.");
    case EOWNERDEAD:
        return Base::Error(Base::Error::OTHER, "Previous owner died.");
    case EPERM:
        return Base::Error(Base::Error::PERMISSION_DENIED, "Operation not permitted.");
    case EPIPE:
        return Base::Error(Base::Error::INVALID_DATA, "Broken pipe.");
    case EPROTO:
        return Base::Error(Base::Error::OTHER, "Protocol error.");
    case EPROTONOSUPPORT:
        return Base::Error(Base::Error::UNSUPPORTED, "Protocol not supported.");
    case EPROTOTYPE:
        return Base::Error(Base::Error::INVALID_INPUT, "Protocol wrong type for socket.");
    case ERANGE:
        return Base::Error(Base::Error::INVALID_INPUT, "Result too large.");
    case EROFS:
        return Base::Error(Base::Error::READ_ONLY_FILESYSTEM, "Read-only file system.");
    case ESPIPE:
        return Base::Error(Base::Error::INVALID_INPUT, "Invalid seek.");
    case ESRCH:
        return Base::Error(Base::Error::NOT_FOUND, "No such process.");
    case ETIME:
        return Base::Error(Base::Error::TIMED_OUT, "Stream ioctl() timeout.");
    case ETIMEDOUT:
        return Base::Error(Base::Error::TIMED_OUT, "Connection timed out.");
    case ETXTBSY:
        return Base::Error(Base::Error::RESOURCE_BUSY, "Text file busy.");
    case EWOULDBLOCK:
        return Base::Error(Base::Error::WOULD_BLOCK, "Operation would block.");
    case EXDEV:
        return Base::Error(Base::Error::INVALID_INPUT, "Cross-device link.");

    default:
        return Error::OTHER;
    }
}

Base::Error fromLastErrno() {
    return fromErrno(errno);
}

} // namespace Karm::Embed::Posix
