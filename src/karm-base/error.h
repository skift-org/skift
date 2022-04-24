#pragma once

namespace Karm::Base
{

struct Error
{
    enum Code
    {
        OK,

        NOT_FOUND,
        PERMISSION_DENIED,
        CONNECTION_REFUSED,
        CONNECTION_RESET,
        HOST_UNREACHABLE,
        NETWORK_UNREACHABLE,
        CONNECTION_ABORTED,
        NOT_CONNECTED,
        ADDR_IN_USE,
        ADDR_NOT_AVAILABLE,
        NETWORK_DOWN,
        BROKEN_PIPE,
        ALREADY_EXISTS,
        WOULD_BLOCK,
        NOT_A_DIRECTORY,
        IS_A_DIRECTORY,
        DIRECTORY_NOT_EMPTY,
        READ_ONLY_FILESYSTEM,
        FILESYSTEM_LOOP,
        STALE_NETWORK_FILE_HANDLE,
        INVALID_INPUT,
        INVALID_DATA,
        TIMED_OUT,
        WRITE_ZERO,
        STORAGE_FULL,
        NOT_SEEKABLE,
        FILESYSTEM_QUOTA_EXCEEDED,
        FILE_TOO_LARGE,
        RESOURCE_BUSY,
        EXECUTABLE_FILE_BUSY,
        DEADLOCK,
        CROSSES_DEVICES,
        TOO_MANY_LINKS,
        INVALID_FILENAME,
        ARGUMENT_LIST_TOO_LONG,
        INTERRUPTED,
        UNSUPPORTED,
        UNEXPECTED_EOF,
        OUT_OF_MEMORY,
        OTHER,
    } _code;

    constexpr Error() : _code(OK) {}
    constexpr Error(Code code) : _code(code) {}

    operator bool() const { return _code == OK; }
    Code code() const { return _code; }
    Error error() const { return *this; }
    Error unwrap() const { return *this; }
};

static constexpr Error::Code OK = Error::OK;

} // namespace Karm::Base
