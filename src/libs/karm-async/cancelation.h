#pragma once

#include <karm-base/res.h>

namespace Karm::Async {

struct Cancelation : Meta::Pinned {
    struct Token {
        Cancelation *_c = nullptr;

        Token() = default;

        Token(Cancelation &c) : _c{&c} {}

        bool canceled() const {
            return _c and _c->canceled();
        }

        Res<> errorIfCanceled() const {
            if (canceled())
                return Error::interrupted("operation canceled");
            return Ok();
        }
    };

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

    Token token() {
        return Token{*this};
    }
};

using Ct = Cancelation::Token;

} // namespace Karm::Async
