#pragma once

#include <coroutine>

#include "pack.h"

namespace Karm::Meta {

template <typename T>
concept Awaiter = requires(T t, std::coroutine_handle<> c) {
    { t.await_ready() } -> Same<bool>;
    { t.await_suspend(c) } -> Contains<void, bool, std::coroutine_handle<>>;
    { t.await_resume() };
};

template <typename T>
concept VoidAwaiter = Awaiter<T> and requires(T t) {
    { t.await_resume() } -> Same<void>;
};

template <typename T>
concept HasMemberAwait = requires(T t) {
    { t.operator co_await() } -> Awaiter;
};

template <typename T>
concept HasFreeAwait = requires(T t) {
    { operator co_await(t) } -> Awaiter;
};

template <typename T>
concept Awaitable =
    HasMemberAwait<T> or
    HasFreeAwait<T> or
    Awaiter<T>;

} // namespace Karm::Meta
