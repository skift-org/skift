#pragma once

import Karm.Core;

#include "bigint.h"

namespace Karm::Math {

struct Expr;

using Rational = BigFrac;

#define FOREACH_UNARY(OP) \
    OP(ABS)               \
    OP(ACOS)              \
    OP(ACOSH)             \
    OP(AIRY_AI)           \
    OP(AIRY_AI_PRIME)     \
    OP(AIRY_BI)           \
    OP(AIRY_BI_PRIME)     \
    OP(ARG)               \
    OP(ASIN)              \
    OP(ASINH)             \
    OP(ATAN)              \
    OP(ATANH)             \
    OP(BOOLE)             \
    OP(BOOLE_EQ_ZERO)     \
    OP(BOOLE_LE_ZERO)     \
    OP(BOOLE_LT_ZERO)     \
    OP(CEIL)              \
    OP(CHI)               \
    OP(CI)                \
    OP(CONJ)              \
    OP(COS)               \
    OP(COSH)              \
    OP(DIGAMMA)           \
    OP(EI)                \
    OP(ELLIPTIC_E)        \
    OP(ELLIPTIC_K)        \
    OP(ERF)               \
    OP(ERFC)              \
    OP(ERFI)              \
    OP(EXP)               \
    OP(FLOOR)             \
    OP(FRESNEL_C)         \
    OP(FRESNEL_S)         \
    OP(GAMMA)             \
    OP(IM)                \
    OP(INVERSE_ERF)       \
    OP(INVERSE_ERFC)      \
    OP(LI)                \
    OP(LN)                \
    OP(LN_GAMMA)          \
    OP(NEG)               \
    OP(NOT)               \
    OP(RE)                \
    OP(RECIP)             \
    OP(SHI)               \
    OP(SI)                \
    OP(SIGN)              \
    OP(SIN)               \
    OP(SINC)              \
    OP(SINH)              \
    OP(SQR)               \
    OP(SQRT)              \
    OP(TAN)               \
    OP(TANH)              \
    OP(UNDEF_AT0)         \
    OP(ZETA)

struct UnaryExpr {
    enum struct _Op {
#define ITER(OP) OP,
        FOREACH_UNARY(ITER)
#undef ITER
    };

    using _Op = _Op;

    _Op _op;
    Box<Expr> _expr;
};

#define FOREACH_BINARY(OP)  \
    OP(ADD)                 \
    OP(AND)                 \
    OP(ATAN2)               \
    OP(BESSEL_I)            \
    OP(BESSEL_J)            \
    OP(BESSEL_K)            \
    OP(BESSEL_Y)            \
    OP(COMPLEX)             \
    OP(DIV)                 \
    OP(EQ)                  \
    OP(GAMMA_INC)           \
    OP(GCD)                 \
    OP(GE)                  \
    OP(GT)                  \
    OP(IM_SINC)             \
    OP(IM_UNDEF_AT0)        \
    OP(IM_ZETA)             \
    OP(LAMBERT_W)           \
    OP(LCM)                 \
    OP(LE)                  \
    OP(LOG)                 \
    OP(LT)                  \
    OP(MAX)                 \
    OP(MIN)                 \
    OP(MOD)                 \
    OP(MUL)                 \
    OP(OR)                  \
    OP(POW)                 \
    OP(POW_RATIONAL)        \
    OP(RANKED_MAX)          \
    OP(RANKED_MIN)          \
    OP(RE_SIGN_NONNEGATIVE) \
    OP(RE_SINC)             \
    OP(RE_UNDEF_AT0)        \
    OP(RE_ZETA)             \
    OP(SUB)

struct BinaryExpr {
    enum struct _Op {
#define ITER(OP) OP,
        FOREACH_BINARY(ITER)
#undef ITER
    };

    using _Op = _Op;

    _Op _op;
    Box<Expr> _lhs;
    Box<Expr> _rhs;
};

#define FOREACH_RELATIONAL(OP) \
    OP(EQ)                     \
    OP(GE)                     \
    OP(GT)                     \
    OP(LE)                     \
    OP(LT)

struct RelationalExpr {
    enum struct _Op {
#define ITER(OP) OP,
        FOREACH_RELATIONAL(ITER)
#undef ITER
    };

    using _Op = _Op;

    _Op _op;
    Box<Expr> _lhs;
    Box<Expr> _rhs;
};

using _Expr = Union<
    Rational,
    UnaryExpr,
    BinaryExpr,
    RelationalExpr>;

struct Expr : _Expr {
    using _Expr::_Expr;
};

} // namespace Karm::Math
