#include <errno.h>

#include "utils.h"

namespace Embed::Posix {

Error fromErrno(int error) {
    if (EOPNOTSUPP != ENOTSUP && error == EOPNOTSUPP) {
        return Error(Error::UNSUPPORTED, "Operation not supported on socket.");
    }

    if (EWOULDBLOCK != EAGAIN && error == EWOULDBLOCK) {
        return Error(Error::WOULD_BLOCK, "Operation would block.");
    }

    switch (error) {
    case E2BIG:
        return Error(Error::ARGUMENT_LIST_TOO_LONG, "Argument list too long.");
    case EACCES:
        return Error(Error::PERMISSION_DENIED, "Permission denied.");
    case EADDRINUSE:
        return Error(Error::ADDR_IN_USE, "Address in use.");
    case EADDRNOTAVAIL:
        return Error(Error::ADDR_NOT_AVAILABLE, "Address not available.");
    case EAFNOSUPPORT:
        return Error(Error::UNSUPPORTED, "Address family not supported.");
    case EAGAIN:
        return Error(Error::RESOURCE_BUSY, "Resource unavailable, try again.");
    case EALREADY:
        return Error(Error::RESOURCE_BUSY, "Connection already in progress.");
    case EBADF:
        return Error(Error::INVALID_INPUT, "Bad file descriptor.");
    case EBADMSG:
        return Error(Error::INVALID_DATA, "Bad message.");
    case EBUSY:
        return Error(Error::RESOURCE_BUSY, "Device or resource busy.");
    case ECANCELED:
        return Error(Error::OTHER, "Operation canceled.");
    case ECHILD:
        return Error(Error::OTHER, "No child processes.");
    case ECONNABORTED:
        return Error(Error::CONNECTION_ABORTED, "Connection aborted.");
    case ECONNREFUSED:
        return Error(Error::CONNECTION_REFUSED, "Connection refused.");
    case ECONNRESET:
        return Error(Error::CONNECTION_RESET, "Connection reset.");
    case EDEADLK:
        return Error(Error::DEADLOCK, "Resource deadlock would occur.");
    case EDESTADDRREQ:
        return Error(Error::INVALID_INPUT, "Destination address required.");
    case EDOM:
        return Error(Error::INVALID_INPUT, "Mathematics argument out of domain of function.");
    case EEXIST:
        return Error(Error::ALREADY_EXISTS, "File exists.");
    case EFAULT:
        return Error(Error::INVALID_INPUT, "Bad address.");
    case EFBIG:
        return Error(Error::FILE_TOO_LARGE, "File too large.");
    case EHOSTUNREACH:
        return Error(Error::HOST_UNREACHABLE, "Host is unreachable.");
    case EIDRM:
        return Error(Error::INVALID_INPUT, "Identifier removed.");
    case EILSEQ:
        return Error(Error::INVALID_INPUT, "Illegal byte sequence.");
    case EINPROGRESS:
        return Error(Error::RESOURCE_BUSY, "Operation in progress.");
    case EINTR:
        return Error(Error::INTERRUPTED, "Interrupted function.");
    case EINVAL:
        return Error(Error::INVALID_INPUT, "Invalid argument.");
    case EIO:
        return Error(Error::INVALID_INPUT, "I/O error.");
    case EISCONN:
        return Error(Error::OTHER, "Socket is connected.");
    case EISDIR:
        return Error(Error::IS_A_DIRECTORY, "Is a directory.");
    case ELOOP:
        return Error(Error::FILESYSTEM_LOOP, "Too many levels of symbolic links.");
    case EMFILE:
        return Error(Error::INVALID_INPUT, "File descriptor value too large.");
    case EMLINK:
        return Error(Error::TOO_MANY_LINKS, "Too many links.");
    case EMSGSIZE:
        return Error(Error::INVALID_INPUT, "Message too large.");
    case ENAMETOOLONG:
        return Error(Error::INVALID_INPUT, "Filename too long.");
    case ENETDOWN:
        return Error(Error::NETWORK_DOWN, "Network is down.");
    case ENETRESET:
        return Error(Error::CONNECTION_RESET, "Connection aborted by network.");
    case ENETUNREACH:
        return Error(Error::NETWORK_UNREACHABLE, "Network unreachable.");
    case ENFILE:
        return Error(Error::LIMIT_REACHED, "Too many files open in system.");
    case ENOBUFS:
        return Error(Error::LIMIT_REACHED, "No buffer space available.");
    case ENODATA:
        return Error(Error::OTHER, "No message is available on the STREAM head read queue.");
    case ENODEV:
        return Error(Error::NOT_FOUND, "No such device.");
    case ENOENT:
        return Error(Error::NOT_FOUND, "No such file or directory.");
    case ENOEXEC:
        return Error(Error::INVALID_DATA, "Executable file format error.");
    case ENOLCK:
        return Error(Error::LIMIT_REACHED, "No locks available.");
    case ENOMEM:
        return Error(Error::OUT_OF_MEMORY, "Not enough space.");
    case ENOMSG:
        return Error(Error::OTHER, "No message of the desired type.");
    case ENOPROTOOPT:
        return Error(Error::INVALID_INPUT, "Protocol not available.");
    case ENOSPC:
        return Error(Error::STORAGE_FULL, "No space left on device.");
    case ENOSR:
        return Error(Error::LIMIT_REACHED, "No STREAM resources.");
    case ENOSTR:
        return Error(Error::INVALID_INPUT, "Not a STREAM.");
    case ENOSYS:
        return Error(Error::UNSUPPORTED, "Functionality not supported.");
    case ENOTCONN:
        return Error(Error::NOT_CONNECTED, "The socket is not connected.");
    case ENOTDIR:
        return Error(Error::INVALID_INPUT, "Not a directory or a symbolic link to a directory.");
    case ENOTEMPTY:
        return Error(Error::DIRECTORY_NOT_EMPTY, "Directory not empty.");
    case ENOTRECOVERABLE:
        return Error(Error::OTHER, "State not recoverable.");
    case ENOTSOCK:
        return Error(Error::INVALID_INPUT, "Not a socket.");
    case ENOTSUP:
        return Error(Error::UNSUPPORTED, "Not supported.");
    case ENOTTY:
        return Error(Error::INVALID_INPUT, "Inappropriate I/O control operation.");
    case ENXIO:
        return Error(Error::NOT_FOUND, "No such device or address.");
    case EOVERFLOW:
        return Error(Error::OTHER, "Value too large to be stored in data type.");
    case EOWNERDEAD:
        return Error(Error::OTHER, "Previous owner died.");
    case EPERM:
        return Error(Error::PERMISSION_DENIED, "Operation not permitted.");
    case EPIPE:
        return Error(Error::INVALID_DATA, "Broken pipe.");
    case EPROTO:
        return Error(Error::OTHER, "Protocol error.");
    case EPROTONOSUPPORT:
        return Error(Error::UNSUPPORTED, "Protocol not supported.");
    case EPROTOTYPE:
        return Error(Error::INVALID_INPUT, "Protocol wrong type for socket.");
    case ERANGE:
        return Error(Error::INVALID_INPUT, "Result too large.");
    case EROFS:
        return Error(Error::READ_ONLY_FILESYSTEM, "Read-only file system.");
    case ESPIPE:
        return Error(Error::INVALID_INPUT, "Invalid seek.");
    case ESRCH:
        return Error(Error::NOT_FOUND, "No such process.");
    case ETIME:
        return Error(Error::TIMED_OUT, "Stream ioctl() timeout.");
    case ETIMEDOUT:
        return Error(Error::TIMED_OUT, "Connection timed out.");
    case ETXTBSY:
        return Error(Error::RESOURCE_BUSY, "Text file busy.");
    case EXDEV:
        return Error(Error::INVALID_INPUT, "Cross-device link.");
    default:
        return Error::OTHER;
    }
}

Error fromLastErrno() {
    return fromErrno(errno);
}

} // namespace Embed::Posix
