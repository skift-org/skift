#pragma once

#include "_prelude.h"

#include "string.h"
#include "try.h"

namespace Karm {

#define FOREACH_ERROR(ERROR)         \
    ERROR(OK)                        \
    ERROR(NOT_FOUND)                 \
    ERROR(PERMISSION_DENIED)         \
    ERROR(CONNECTION_REFUSED)        \
    ERROR(CONNECTION_RESET)          \
    ERROR(HOST_UNREACHABLE)          \
    ERROR(NETWORK_UNREACHABLE)       \
    ERROR(CONNECTION_ABORTED)        \
    ERROR(NOT_CONNECTED)             \
    ERROR(ADDR_IN_USE)               \
    ERROR(ADDR_NOT_AVAILABLE)        \
    ERROR(NETWORK_DOWN)              \
    ERROR(BROKEN_PIPE)               \
    ERROR(ALREADY_EXISTS)            \
    ERROR(WOULD_BLOCK)               \
    ERROR(NOT_A_DIRECTORY)           \
    ERROR(IS_A_DIRECTORY)            \
    ERROR(DIRECTORY_NOT_EMPTY)       \
    ERROR(READ_ONLY_FILESYSTEM)      \
    ERROR(FILESYSTEM_LOOP)           \
    ERROR(STALE_NETWORK_FILE_HANDLE) \
    ERROR(INVALID_INPUT)             \
    ERROR(INVALID_DATA)              \
    ERROR(TIMED_OUT)                 \
    ERROR(WRITE_ZERO)                \
    ERROR(STORAGE_FULL)              \
    ERROR(NOT_SEEKABLE)              \
    ERROR(FILESYSTEM_QUOTA_EXCEEDED) \
    ERROR(FILE_TOO_LARGE)            \
    ERROR(RESOURCE_BUSY)             \
    ERROR(EXECUTABLE_FILE_BUSY)      \
    ERROR(DEADLOCK)                  \
    ERROR(CROSSES_DEVICES)           \
    ERROR(TOO_MANY_LINKS)            \
    ERROR(INVALID_FILENAME)          \
    ERROR(ARGUMENT_LIST_TOO_LONG)    \
    ERROR(INTERRUPTED)               \
    ERROR(UNSUPPORTED)               \
    ERROR(UNEXPECTED_EOF)            \
    ERROR(OUT_OF_MEMORY)             \
    ERROR(LIMIT_REACHED)             \
    ERROR(OTHER)

struct [[nodiscard]] Error {
    enum class Code {
#define ITER(NAME) NAME,
        FOREACH_ERROR(ITER)
#undef ITER
    } _code;
    Str _msg;

    using enum Code;

    constexpr Error() : _code(Code::OK) {}
    constexpr Error(Code code) : _code(code), _msg("") {}
    constexpr Error(Code code, Str msg) : _code(code), _msg(msg) {}

    operator bool() const { return _code == Code::OK; }

    constexpr Code code() const { return _code; }

    constexpr Error none() const { return *this; }

    constexpr Error unwrap() const { return *this; }

    constexpr Str msg() {
        if (_msg.len() > 0) {
            return _msg;
        }

        switch (_code) {
#define ITER(NAME) \
    case NAME:     \
        return #NAME;
            FOREACH_ERROR(ITER)
#undef ITER
        default:
            return "UNKNOWN";
        }
    }
};

static inline constexpr Error OK = {Error::OK};

static_assert(Tryable<Error>);

#undef FOREACH_ERROR

} // namespace Karm
