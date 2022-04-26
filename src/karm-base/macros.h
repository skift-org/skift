#pragma once

#include "_prelude.h"

#define try$(EXPR)                \
    ({                            \
        auto __expr = (EXPR);     \
        if (!__expr) {            \
            return __expr.none(); \
        }                         \
        __expr.unwrap();          \
    })

namespace Karm::Base {

template <typename T>
concept Tryable = requires(T t) {
    {!t};
    {t.none()};
    {t.unwrap()};
};

} // namespace Karm::Base
