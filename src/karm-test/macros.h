#pragma once

#include <karm-base/macros.h>
#include <karm-base/ordr.h>

#include "driver.h"
#include "test.h"

namespace Karm::Test {

#define test$(ID)                                                                   \
    ::Karm::Error var$(_testFunc)([[maybe_unused]] ::Karm::Test::Driver & _driver); \
    ::Karm::Test::Test var$(_test){ID, var$(_testFunc)};                            \
    ::Karm::Error var$(_testFunc)([[maybe_unused]] ::Karm::Test::Driver & _driver)

#define describe$(DESCR) \
    condDefer$(_driver.beginDescribe(DESCR), _driver.endDescribe())

#define it$(DESCR) \
    condDefer$(_driver.beginIt(DESCR), _driver.endIt())

#define __expect$(LHS, RHS, OP)                         \
    ({                                                  \
        auto __lhs = (LHS);                             \
        auto __rhs = (RHS);                             \
                                                        \
        if (!Op::OP(__lhs, __rhs)) {                    \
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
