export module Karm.Core:async.queue;

import :async.awaiter;
import :base.list;
import :base.slice;
import :base.vec;

namespace Karm::Async {

export template <typename T>
struct Queue {
    struct _Listener {
        LlItem<_Listener> item;
        Opt<T> value;
        virtual ~_Listener() = default;
        virtual void complete() = 0;
    };

    Vec<T> _buf;
    Ll<_Listener> _listeners;

    Queue() = default;

    template <typename... Ts>
    void emplace(Ts&&... arg) {
        if (_listeners.empty()) {
            _buf.emplaceBack(std::forward<Ts>(arg)...);
            return;
        }

        if (not empty())
            panic("queue should be empty");

        auto listener = _listeners.detach(_listeners.head());
        listener->value.emplace(std::forward<Ts>(arg)...);
        listener->complete();
    }

    void enqueue(T item) {
        emplace(std::move(item));
    }

    template <Receiver<T> R>
    struct _GetOperation : private _Listener {
        using _Listener::value;

        Queue* _q;
        R _r;

        _GetOperation(Queue* q, R r)
            : _q{q}, _r{std::move(r)} {}

        bool start() {
            if (_q->empty()) {
                _q->_listeners.append(this, _q->_listeners.head());
                return false;
            }

            if (not _q->_listeners.empty())
                panic("listeners should be empty");

            _r.recv(INLINE, _q->_buf.popFront());
            return true;
        }

        void complete() override {
            _r.recv(LATER, value.take());
        }
    };

    struct _GetSender {
        using Inner = T;
        Queue* _q;

        auto connect(Receiver<T> auto r) -> _GetOperation<decltype(r)> {
            return {_q, std::move(r)};
        }
    };

    auto dequeueAsync() {
        return _GetSender{this};
    }

    bool empty() {
        return isEmpty(_buf);
    }

    Opt<T> dequeue() {
        if (empty())
            return NONE;
        return _buf.popFront();
    }
};

} // namespace Karm::Async
