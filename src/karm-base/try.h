#pragma once

#include <karm-meta/cvrp.h>

#include "_prelude.h"

#define try$(EXPR)                \
    ({                            \
        auto __expr = (EXPR);     \
        if (!__expr) {            \
            return __expr.none(); \
        }                         \
        __expr.take();            \
    })

namespace Karm {

template <typename T>
concept Tryable = requires(T t) {
    {!t};
    {t.none()};
    {t.unwrap()};
};

auto try_or(Tryable auto opt, auto default_value) -> Meta::RemoveRef<decltype(opt.unwrap())> {
    if (!opt) {
        return default_value;
    }

    return opt.unwrap();
}

auto try_or_else(Tryable auto opt, auto default_value) -> decltype(opt.unwrap()) {
    if (!opt) {
        return default_value();
    }

    return opt.unwrap();
}

} // namespace Karm
