export module Karm.Core:async.promise;

import :base.list;
import :base.rc;
import :base.res;
import :async.base;

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

    T& unwrap() {
        if (not _value.has()) [[unlikely]]
            panic("promise not resolved");
        return _value.unwrap();
    }

    void attach(Listener& listener) {
        _queue.append(&listener, _queue.tail());
    }

    void detach(Listener& listener) {
        _queue.detach(&listener);
    }
};

export template <typename T>
struct _Future {
    using Inner = T;

    Rc<State<T>> _state;

    template <Receiver<T> R>
    struct _Operation :
        State<T>::Listener,
        Meta::Pinned {

        Rc<State<T>> _state;
        R _r;

        _Operation(Rc<State<T>> state, R r)
            : _state{std::move(state)}, _r{std::move(r)} {}

        ~_Operation() {
            _state->detach(*this);
        }

        void resume() {
            _r.recv(Async::LATER, _state->unwrap());
        }

        bool start() {
            if (_state->has()) {
                _r.recv(Async::INLINE, _state->unwrap());
                return true;
            }

            _state->attach(*this);
            return false;
        }
    };

    template <Receiver<T> R>
    auto connect(R r) {
        return _Operation<R>{_state, std::move(r)};
    }
};

export template <typename V = None, typename E = Error>
using Future = _Future<Res<V, E>>;

export template <typename T>
struct _Promise : Meta::NoCopy {
    Opt<Rc<State<T>>> _state;

    _Promise() : _state{makeRc<State<T>>()} {}

    void resolve(T value) {
        if (not _state.has()) [[unlikely]]
            panic("promise already resolved");
        _state.take()->set(std::move(value));
    }

    _Future<T> future() {
        return {_state.unwrap()};
    }
};

export template <typename V = None, typename E = Error>
using Promise = _Promise<Res<V, E>>;

} // namespace Karm::Async
