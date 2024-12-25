#pragma once

#include <karm-base/list.h>
#include <karm-base/rc.h>
#include <karm-base/res.h>

#include "base.h"

namespace Karm::Async {

template <typename T>
struct State {
    struct Listener {
        LlItem<Listener> item;
        virtual ~Listener() = default;
        virtual void resume() = 0;
    };

    Opt<T> _value;
    Ll<Listener> _queue;

    void set(T value) {
        if (_value.has()) [[unlikely]]
            panic("promise already resolved");
        _value = std::move(value);
        wake();
    }

    void wake() {
        while (_queue.head()) {
            auto listener = _queue.detach(_queue.head());
            listener->resume();
        }
    }

    bool has() const {
        return _value.has();
    }

    T &unwrap() {
        if (not _value.has()) [[unlikely]]
            panic("promise not resolved");
        return _value.unwrap();
    }

    void attach(Listener &listener) {
        _queue.append(&listener, _queue.tail());
    }

    void detach(Listener &listener) {
        _queue.detach(&listener);
    }
};

template <typename T>
struct _Future {
    using Inner = T;

    Strong<State<T>> _state;

    template <Receiver<T> R>
    struct _Operation :
        public State<T>::Listener {

        Strong<State<T>> _state;
        R _r;

        _Operation(Strong<State<T>> state, R r)
            : _state{std::move(state)}, _r{std::move(r)} {}

        ~_Operation() {
            _state->detach(*this);
        }

        void resume() {
            _r.recv(Async::LATER, _state->unwrap());
        }

        bool start() {
            if (not _state->has()) {
                _state->attach(*this);
                return false;
            }
            _r.recv(Async::INLINE, _state->unwrap());
            return true;
        }
    };

    template <Receiver<T> R>
    auto connect(R r) {
        return _Operation<R>{_state, std::move(r)};
    }
};

template <typename V = None, typename E = Error>
using Future = _Future<Res<V, E>>;

template <typename T>
struct _Promise : Meta::NoCopy {
    Opt<Strong<State<T>>> _state;

    _Promise() : _state{makeStrong<State<T>>()} {}

    void resolve(T value) {
        if (not _state.has()) [[unlikely]]
            panic("promise already resolved");
        _state.take()->set(std::move(value));
    }

    _Future<T> future() {
        return _Future<T>{_state.unwrap()};
    }
};

template <typename V = None, typename E = Error>
using Promise = _Promise<Res<V, E>>;

} // namespace Karm::Async
