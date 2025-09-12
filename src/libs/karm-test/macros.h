#pragma once

#include "driver.h"
#include "test.h"

namespace Karm::Test {

#define test$(ID)                                                                     \
    static ::Karm::Res<> var$(func)([[maybe_unused]] ::Karm::Test::Driver & _driver); \
    static ::Karm::Test::Test var$(test){ID, var$(func)};                             \
    static ::Karm::Res<> var$(func)([[maybe_unused]] ::Karm::Test::Driver & _driver)

#define testAsync$(ID)                                                                             \
    static ::Karm::Async::Task<> var$(funcAsync)([[maybe_unused]] ::Karm::Test::Driver & _driver); \
    static ::Karm::Test::Test var$(test){ID, var$(funcAsync)};                                     \
    static ::Karm::Async::Task<> var$(funcAsync)([[maybe_unused]] ::Karm::Test::Driver & _driver)

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

#define __co_expect$(LHS, RHS, OP, OPSTR)                    \
    ({                                                       \
        /* Make sure LHS and RHS are evaluated once */       \
        auto __lhs = (LHS);                                  \
        auto __rhs = (RHS);                                  \
                                                             \
        if (not((__lhs)OP(__rhs))) {                         \
            co_return _driver.unexpect(__lhs, __rhs, OPSTR); \
        }                                                    \
    })

#define co_expect$(EXPR) __co_expect$(EXPR, true, ==, "=")

#define co_expectNot$(EXPR) __co_expect$(EXPR, true, !=, "≠")

#define co_expectEq$(LHS, RHS) __co_expect$(LHS, RHS, ==, "=")

#define co_expectNe$(LHS, RHS) __co_expect$(LHS, RHS, !=, "≠")

#define co_expectLt$(LHS, RHS) __co_expect$(LHS, RHS, <, "<")

#define co_expectGt$(LHS, RHS) __co_expect$(LHS, RHS, >, ">")

#define co_expectLteq$(LHS, RHS) __co_expect$(LHS, RHS, <=, "≤")

#define co_expectGteq$(LHS, RHS) __co_expect$(LHS, RHS, >=, "≥")

} // namespace Karm::Test
