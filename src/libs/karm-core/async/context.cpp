export module Karm.Core:async.context;

import :async.cancellation;
import :base.time;

namespace Karm::Async {

export struct Context {
    Opt<Cancellation::Token> _token;
    Instant _deadline = Instant::END_OF_TIME;

    static Context background() {
        return Context{};
    }

    Context withDeadline(Instant deadline) {
        auto copy = *this;
        copy._deadline = deadline;
        return copy;
    }

    Res<> errorIfCanceled() {
        if (_token)
            return _token->errorIfCanceled();
        return Ok();
    }

    Tuple<Context, Rc<Cancellation>> withCancel() {
        auto copy = *this;
        auto [cancelation, token] = Cancellation::create();
        copy._token = token;
        return {copy, cancelation};
    }
};

} // namespace Karm::Async
