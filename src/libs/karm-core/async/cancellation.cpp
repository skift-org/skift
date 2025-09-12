export module Karm.Core:async.cancellation;

import :base.rc;
import :base.res;

namespace Karm::Async {

export struct Cancellation : Meta::Pinned {
    struct Token {
        Rc<Cancellation> _c;

        Token(Rc<Cancellation> c) : _c{c} {}

        bool canceled() const {
            return _c->canceled();
        }

        Res<> errorIfCanceled() const {
            if (canceled())
                return Error::interrupted("operation canceled");
            return Ok();
        }
    };

    static Tuple<Rc<Cancellation>, Token> create() {
        auto cancellation = makeRc<Cancellation>();
        return {cancellation, Token{cancellation}};
    }

    bool _canceled = false;

    void cancel() {
        _canceled = true;
    }

    void reset() {
        _canceled = false;
    }

    bool canceled() const {
        return _canceled;
    }
};

export using Ct = Cancellation::Token;

} // namespace Karm::Async
