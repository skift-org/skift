export module Karm.Core:async.one;

import :async.base;

namespace Karm::Async {

export template <typename T>
struct One {
    using Inner = T;
    T _t;

    template <Receiver<T> R>
    auto connect(R r) {
        struct Operation {
            T _t;
            R _r;

            bool start() {
                _r.recv(Async::INLINE, _t);
                return true;
            }
        };

        return Operation{_t, r};
    }
};

} // namespace Karm::Async
