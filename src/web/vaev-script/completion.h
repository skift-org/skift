#pragma once

#include <karm-base/res.h>

#include "value.h"

namespace Vaev::Script {

// https://tc39.es/ecma262/#sec-completion-record-specification-type
struct [[nodiscard]] Completion {
    enum struct _Type {
        NORMAL,
        BREAK,
        CONTINUE,
        RETURN,
        THROW,
    };

    using enum _Type;

    _Type type = _Type::NORMAL;
    Value value = undefined;
    String target = u""_s16;

    static Completion normal(Value value) {
        return {NORMAL, value};
    }

    static Completion break_(String target) {
        return {BREAK, undefined, target};
    }

    static Completion continue_(String target) {
        return {CONTINUE, undefined, target};
    }

    static Completion return_(Value value) {
        return {RETURN, value};
    }

    static Completion throw_(Value value) {
        return {THROW, value};
    }

    bool operator==(_Type t) const {
        return type == t;
    }

    void repr(Io::Emit& e) const {
        switch (type) {
        case NORMAL:
            e("normal({})", value);
            break;

        case BREAK:
            e("break({})", target);
            break;

        case CONTINUE:
            e("continue({})", target);
            break;

        case RETURN:
            e("return({})", value);
            break;

        case THROW:
            e("throw({})", value);
            break;
        }
    }
};

template <typename T = None>
using Except = Res<T, Completion>;

} // namespace Vaev::Script
