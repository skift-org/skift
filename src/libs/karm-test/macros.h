#pragma once

#include <karm-base/macros.h>

#include "driver.h"
#include "test.h"

namespace Karm::Test {

#define test$(ID)                                                                   \
    static ::Karm::Res<> var$(ID)([[maybe_unused]] ::Karm::Test::Driver & _driver); \
    static ::Karm::Test::Test var$(_test){#ID, var$(ID)};                           \
    static ::Karm::Res<> var$(ID)([[maybe_unused]] ::Karm::Test::Driver & _driver)

#define __expect$(LHS, RHS, OP)                         \
    ({                                                  \
        /* Make sure LHS and RHS are evaluated once */  \
        auto __lhs = (LHS);                             \
        auto __rhs = (RHS);                             \
                                                        \
        if (not((__lhs)OP(__rhs))) {                    \
            return _driver.unexpect(__lhs, __rhs, #OP); \
        }                                               \
    })

#define expect$(EXPR) __expect$(EXPR, true, ==)

#define expectNot$(EXPR) __expect$(EXPR, true, !=)

#define expectEq$(LHS, RHS) __expect$(LHS, RHS, ==)

#define expectNe$(LHS, RHS) __expect$(LHS, RHS, !=)

#define expectLt$(LHS, RHS) __expect$(LHS, RHS, <)

#define expectGt$(LHS, RHS) __expect$(LHS, RHS, >)

#define expectLteq$(LHS, RHS) __expect$(LHS, RHS, <=)

#define expectGteq$(LHS, RHS) __expect$(LHS, RHS, >=)

} // namespace Karm::Test
