export module Karm.Core:base.error;

import :base.cstr;
import :base.panic;
import :base.try_;

namespace Karm {

#define FOREACH_ERROR(ERROR)                                  \
    ERROR(_OK, _ok)                                           \
    ERROR(SKIPPED, skipped)                                   \
    ERROR(NOT_IMPLEMENTED, notImplemented)                    \
    ERROR(NOT_FOUND, notFound)                                \
    ERROR(PERMISSION_DENIED, permissionDenied)                \
    ERROR(CONNECTION_REFUSED, connectionRefused)              \
    ERROR(CONNECTION_RESET, connectionReset)                  \
    ERROR(HOST_UNREACHABLE, hostUnreachable)                  \
    ERROR(NETWORK_UNREACHABLE, networkUnreachable)            \
    ERROR(CONNECTION_ABORTED, connectionAborted)              \
    ERROR(NOT_CONNECTED, notConnected)                        \
    ERROR(ADDR_IN_USE, addrInUse)                             \
    ERROR(ADDR_NOT_AVAILABLE, addrNotAvailable)               \
    ERROR(NETWORK_DOWN, networkDown)                          \
    ERROR(BROKEN_PIPE, brokenPipe)                            \
    ERROR(ALREADY_EXISTS, alreadyExists)                      \
    ERROR(WOULD_BLOCK, wouldBlock)                            \
    ERROR(NOT_A_DIRECTORY, notADirectory)                     \
    ERROR(IS_A_DIRECTORY, isADirectory)                       \
    ERROR(DIRECTORY_NOT_EMPTY, directoryNotEmpty)             \
    ERROR(READ_ONLY_FILESYSTEM, readOnlyFilesystem)           \
    ERROR(FILESYSTEM_LOOP, filesystemLoop)                    \
    ERROR(STALE_NETWORK_FILE_HANDLE, staleNetworkFileHandle)  \
    ERROR(INVALID_INPUT, invalidInput)                        \
    ERROR(INVALID_DATA, invalidData)                          \
    ERROR(TIMED_OUT, timedOut)                                \
    ERROR(WRITE_ZERO, writeZero)                              \
    ERROR(STORAGE_FULL, storageFull)                          \
    ERROR(NOT_SEEKABLE, notSeekable)                          \
    ERROR(FILESYSTEM_QUOTA_EXCEEDED, filesystemQuotaExceeded) \
    ERROR(FILE_TOO_LARGE, fileTooLarge)                       \
    ERROR(RESOURCE_BUSY, resourceBusy)                        \
    ERROR(EXECUTABLE_FILE_BUSY, executableFileBusy)           \
    ERROR(DEADLOCK, deadlock)                                 \
    ERROR(CROSSES_DEVICES, crossesDevices)                    \
    ERROR(TOO_MANY_LINKS, tooManyLinks)                       \
    ERROR(INVALID_FILENAME, invalidFilename)                  \
    ERROR(INVALID_HANDLE, invalidHandle)                      \
    ERROR(ARGUMENT_LIST_TOO_LONG, argumentListTooLong)        \
    ERROR(ARITHMETIC, arithmetic)                             \
    ERROR(INTERRUPTED, interrupted)                           \
    ERROR(UNSUPPORTED, unsupported)                           \
    ERROR(UNEXPECTED_EOF, unexpectedEof)                      \
    ERROR(OUT_OF_MEMORY, outOfMemory)                         \
    ERROR(LIMIT_REACHED, limitReached)                        \
    ERROR(OTHER, other)

export struct [[nodiscard]] Error {
    enum struct Code {
#define ITER(NAME, _) NAME,
        FOREACH_ERROR(ITER)
#undef ITER

            _LEN,
    } _code;

    char const* _msg = nullptr;

    using enum Code;

#define ITER(CODE, NAME) \
    static constexpr Error NAME(char const* msg = "") { return Error(CODE, msg); }
    FOREACH_ERROR(ITER)
#undef ITER

    constexpr Error() : _code(Code::OTHER), _msg("unknown error") {}

    constexpr Error(Code code, char const* msg) : _code(code), _msg(msg) {}

    constexpr Code code() const { return _code; }

    constexpr Error none() const { return *this; }

    constexpr char const* msg() const {
        if (_msg != nullptr and std::strlen(_msg) > 0) {
            return _msg;
        }

        switch (_code) {
#define ITER(NAME, _) \
    case NAME:        \
        return #NAME;
            FOREACH_ERROR(ITER)
#undef ITER
        default:
            return "UNKNOWN";
        }
    }

    constexpr bool operator==(Error const& other) const {
        return _code == other._code;
    }

    constexpr bool operator==(Code const& code) const {
        return _code == code;
    }
};

#undef FOREACH_ERROR

} // namespace Karm
