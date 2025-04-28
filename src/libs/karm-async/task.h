#pragma once

#include <karm-base/base.h>
#include <karm-base/res.h>

#include "awaiter.h"

namespace Karm::Async {

enum struct Cfp {
    INDETERMINATE,
    PAST_START,
    PAST_SUSPEND
};

template <typename T>
struct [[nodiscard, clang::coro_return_type, clang::coro_lifetimebound]] _Task {
    using Inner = T;

    struct promise_type {
        Continuation<T>* _resume = nullptr;
        Cfp _cfp = Cfp::INDETERMINATE;

        _Task get_return_object() {
            return _Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        void unhandled_exception() {
            panic("unhandled exception in coroutine");
        }

        void return_value(T value) {
            _resume->value = std::move(value);
        }

        template <typename U>
        void return_value(U&& value) {
            _resume->value.emplace(std::forward<U>(value));
        }

        std::suspend_always initial_suspend() {
            return {};
        }

        auto final_suspend() noexcept {
            struct Awaiter {
                promise_type* _promise;

                bool await_ready() noexcept {
                    return false;
                }

                void await_suspend(std::coroutine_handle<void>) noexcept {
                    auto cfp = std::exchange(_promise->_cfp, Cfp::PAST_SUSPEND);
                    if (cfp == Cfp::PAST_START)
                        _promise->_resume->resume();
                }

                void await_resume() noexcept {
                    panic("coroutine resumed from final suspension point");
                }
            };

            return Awaiter{this};
        }
    };

    std::coroutine_handle<promise_type> _coro;

    _Task(std::coroutine_handle<promise_type> coro = nullptr)
        : _coro{coro} {}

    _Task(_Task const& other) = delete;

    _Task(_Task&& other)
        : _coro(std::exchange(other._coro, nullptr)) {}

    _Task& operator=(_Task const& other) = delete;

    _Task& operator=(_Task&& other) {
        std::swap(_coro, other._coro);
        return *this;
    }

    ~_Task() {
        if (_coro)
            _coro.destroy();
    }

    template <typename R>
    auto connect(R r) {
        struct Operation : Continuation<T> {
            using Continuation<T>::value;

            _Task _t;
            R _r;

            Operation(_Task t, R r)
                : _t{std::move(t)}, _r{std::move(r)} {}

            bool start() {
                auto coro = _t._coro;
                promise_type& prom = coro.promise();
                prom._resume = this;

                coro.resume();
                auto cfp = std::exchange(prom._cfp, Cfp::PAST_START);
                if (cfp != Cfp::PAST_SUSPEND)
                    return false;

                _r.recv(Async::INLINE, value.take());
                return true;
            }

            void resume() override {
                _r.recv(Async::LATER, value.take());
            }
        };

        return Operation{std::move(*this), std::move(r)};
    }
};

template <typename V = None, typename E = Error>
using Task = _Task<Res<V, E>>;

template <Sender S>
_Task<typename S::Inner> makeTask(S s) {
    co_return co_await std::move(s);
}

} // namespace Karm::Async
