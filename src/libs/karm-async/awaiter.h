#pragma once

#include <karm-base/opt.h>
#include <karm-meta/nocopy.h>

#include "base.h"

namespace Karm::Async {

template <typename T>
struct Continuation {
    Opt<T> value = NONE;
    virtual ~Continuation() = default;
    virtual void resume() = 0;
};

template <Sender S>
struct [[nodiscard]] Awaiter :
    public Continuation<typename S::Inner>,
    Meta::Pinned,
    Meta::NoCopy {

    using Continuation<typename S::Inner>::value;

    struct _Receiver {
        Continuation<typename S::Inner> &_c;

        void recv(Inline, typename S::Inner t) {
            _c.value = std::move(t);
        }

        void recv(Later, typename S::Inner t) {
            _c.value = std::move(t);
            _c.resume();
        }
    };

    OperationOf<S, _Receiver> _op;
    std::coroutine_handle<> _coro = nullptr;

    Awaiter(S s)
        : _op{s.connect(_Receiver{*this})} {}

    bool await_ready() const {
        return false;
    }

    bool await_suspend(std::coroutine_handle<> coro) {
        _coro = coro;
        return not _op.start();
    }

    typename S::Inner await_resume() {
        return value.take();
    }

    void resume() override {
        _coro.resume();
    }
};

template <Sender S>
Awaiter<S> operator co_await(S s) {
    return Awaiter<S>{std::move(s)};
}

} // namespace Karm::Async
