#pragma once

#include "_prelude.h"

#include "keywords.h"
#include "panic.h"
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
    enum struct Code {
#define ITER(NAME) NAME,
        FOREACH_ERROR(ITER)
#undef ITER
    } _code;

    char const *_msg = nullptr;

    using enum Code;

    constexpr Error(None) : _code(Code::OTHER), _msg("unwraping none") {}
    constexpr Error(Code code) : _code(code) {}
    constexpr Error(char const *msg) : _code(OTHER), _msg(msg) {}
    constexpr Error(Code code, char const *msg) : _code(code), _msg(msg) {}

    constexpr explicit operator bool() const { return _code == Code::OK; }

    constexpr Code code() const { return _code; }

    constexpr Error none() const { return *this; }

    constexpr bool unwrap(char const *msg = "unwraping an error") const {
        if (_code != OK) {
            panic(msg);
        }
        return true;
    }

    constexpr bool take() const {
        if (_code != OK) {
            panic("take() called on an error");
        }
        return true;
    }

    constexpr char const *msg() {
        if (_msg != nullptr && strlen(_msg) > 0) {
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
