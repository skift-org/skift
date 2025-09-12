export module Karm.Core:meta.async;

import :base.std;
import :meta.traits;
import :meta.pack;

namespace Karm::Meta {

export template <typename T>
concept Awaiter = requires(T t, std::coroutine_handle<> c) {
    { t.await_ready() } -> Same<bool>;
    { t.await_suspend(c) } -> Contains<void, bool, std::coroutine_handle<>>;
    { t.await_resume() };
};

export template <typename T>
concept VoidAwaiter = Awaiter<T> and requires(T t) {
    { t.await_resume() } -> Same<void>;
};

export template <typename T>
concept HasMemberAwait = requires(T t) {
    { t.operator co_await() } -> Awaiter;
};

export template <typename T>
concept HasFreeAwait = requires(T t) {
    { operator co_await(t) } -> Awaiter;
};

export template <typename T>
concept Awaitable =
    HasMemberAwait<T> or
    HasFreeAwait<T> or
    Awaiter<T>;

} // namespace Karm::Meta
