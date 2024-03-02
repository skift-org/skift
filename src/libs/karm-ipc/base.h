#pragma once

#include <karm-base/func.h>
#include <karm-io/bscan.h>
#include <karm-io/pack.h>
#include <karm-logger/logger.h>
#include <karm-sys/context.h>
#include <karm-sys/socket.h>

namespace Karm::Ipc {

struct Null {
    template <typename I, u64 uid, typename R, typename... Args>
    auto invoke(Args &&...) {
        return R{};
    }
};

template <typename T>
concept Interface = requires(T &t) {
    { T::_UID } -> Meta::Convertible<long long>;
    { T::_NAME } -> Meta::Convertible<char const *>;
    typename T::template _Client<Null>;
};

struct Message {
    static constexpr auto ERROR = 0xffffffffffffffff;
    static constexpr auto END = 0xfffffffffffffffe;

    Buf<Byte> data;
    Buf<Sys::Handle> handles;
};

} // namespace Karm::Ipc
