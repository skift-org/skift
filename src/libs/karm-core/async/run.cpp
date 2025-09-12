export module Karm.Core:async.run;

import :base.opt;
import :async.base;

namespace Karm::Async {

export template <Sender S>
typename S::Inner run(S s, auto wait) {
    Opt<typename S::Inner> ret;

    struct Receiver {
        Opt<typename S::Inner>& _ret;

        void recv(InlineOrLater, typename S::Inner r) { _ret = r; }
    };

    auto op = s.connect(Receiver{ret});
    if (op.start())
        return ret.take();
    while (not ret.has())
        wait();
    return ret.take();
}

export template <Sender S>
typename S::Inner run(S s) {
    Opt<typename S::Inner> ret;

    struct Receiver {
        Opt<typename S::Inner>& _ret;

        void recv(InlineOrLater, typename S::Inner r) { _ret = r; }
    };

    auto op = s.connect(Receiver{ret});
    if (not op.start()) [[unlikely]]
        panic("run() called on pending operation without a wait function");
    return ret.take();
}

export template <Sender S, typename Cb>
void detach(S s, Cb cb) {
    struct _Holder {
        virtual void finalize(typename S::Inner) = 0;
        virtual ~_Holder() = default;
    };

    struct Receiver {
        _Holder* _h;

        void recv(InlineOrLater, typename S::Inner r) {
            _h->finalize(std::move(r));
            // After this point, `_d` is invalid and `this` is dangling.
        }
    };

    struct Holder : _Holder {
        using Op = OperationOf<S, Receiver>;
        Op _op;
        Cb _cb;

        Holder(S s, Cb cb)
            : _op{s.connect(Receiver{this})}, _cb{std::move(cb)} {}

        void finalize(typename S::Inner r) override {
            auto cb = std::move(_cb);
            delete this;
            cb(r);
        }

        void start() {
            _op.start();
        }
    };

    (new Holder(std::move(s), std::move(cb)))->start();
}

export template <Sender S>
void detach(S s) {
    detach(std::move(s), [](auto) {
    });
}

} // namespace Karm::Async
