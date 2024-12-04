#pragma once

#include <karm-meta/cvrp.h>

#include "base.h"

namespace Karm {

template <typename T>
concept Takeable = requires(T t) {
    { t.take() };
};

// MARK: Unwrapable ------------------------------------------------------------

template <typename T>
concept Unwrapable = requires(T t) {
    { t.unwrap() };
};

// MARK: Tryable ---------------------------------------------------------------

template <typename T>
concept Tryable = requires(T t) {
    { not static_cast<bool>(t) };
    { t.none() };
} and Unwrapable<T> and Takeable<T>;

} // namespace Karm

// Give us a symbole to break one when debbuging error handling.
// This is a no-op in release mode.
#if defined(__ck_debug__) and !defined(KARM_DISABLE_TRY_FAIL_HOOK)
extern "C" void __try_failed();
#    define __tryFail() __try_failed()
#else
#    define __tryFail() /* NOP */
#endif

#define __try$(EXPR, RET, AWAIT) ({                   \
    auto __expr = AWAIT(EXPR);                        \
    if (not static_cast<bool>(__expr)) [[unlikely]] { \
        __tryFail();                                  \
        RET __expr.none();                            \
    }                                                 \
    __expr.take();                                    \
})

#define try$(EXPR) __try$(EXPR, return, )

#define co_try$(EXPR) __try$(EXPR, co_return, )

#define co_trya$(EXPR) __try$(EXPR, co_return, co_await)
