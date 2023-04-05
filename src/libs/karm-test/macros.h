#pragma once

#include <karm-base/macros.h>
#include <karm-base/ordr.h>

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
        if (not Op::OP(__lhs, __rhs)) {                 \
            return _driver.unexpect(__lhs, __rhs, #OP); \
        }                                               \
    })

#define expect$(EXPR) __expect$(EXPR, true, eq)

#define expectNot$(EXPR) __expect$(EXPR, true, ne)

#define expectEq$(LHS, RHS) __expect$(LHS, RHS, eq)

#define expectNe$(LHS, RHS) __expect$(LHS, RHS, ne)

#define expectLt$(LHS, RHS) __expect$(LHS, RHS, lt)

#define expectGt$(LHS, RHS) __expect$(LHS, RHS, gt)

#define expectLteq$(LHS, RHS) __expect$(LHS, RHS, lteq)

#define expectGteq$(LHS, RHS) __expect$(LHS, RHS, gteq)

}; // namespace Karm::Test
