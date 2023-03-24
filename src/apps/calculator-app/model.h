#pragma once

#include <karm-ui/react.h>

namespace Calculator {

enum struct Operator;

struct State {
    Operator op;
    isize lhs;
    isize rhs;
    bool hasRhs;

    isize mem;
    bool hasMem;
};

enum struct Operator {
    NONE,

    ADD,
    SUB,
    MULT,
    DIV,
    MOD,

    // UNARY
    SQUARE,
    SQRT,
    INVERT_SIGN,
    TO_PERCENT,
    RESIPROCAL,
};

inline Str toFmt(Operator op) {
    switch (op) {
    case Operator::NONE:
        return "{}";
    case Operator::ADD:
        return "{} +";
    case Operator::SUB:
        return "{} -";
    case Operator::MULT:
        return "{} *";
    case Operator::DIV:
        return "{} /";
    case Operator::MOD:
        return "{} %";
    case Operator::SQUARE:
        return "{}²";
    case Operator::SQRT:
        return "√{}";
    case Operator::INVERT_SIGN:
        return "-{}";
    case Operator::TO_PERCENT:
        return "{}%";
    case Operator::RESIPROCAL:
        return "1/{}";
    }
}

inline bool isUnary(Operator op) {
    switch (op) {
    case Operator::SQUARE:
    case Operator::SQRT:
    case Operator::INVERT_SIGN:
    case Operator::TO_PERCENT:
    case Operator::RESIPROCAL:
        return true;
    default:
        return false;
    }
}

using Number = isize;

struct BackspaceAction {};

struct EqualAction {};

struct ClearAction {};

struct ClearAllAction {};

struct MemClearAction {};

struct MemRecallAction {};

struct MemAddAction {};

struct MemSubAction {};

struct MemStoreAction {};

struct EnterDecimalAction {};

using Actions = Var<
    Operator, Number, BackspaceAction, EqualAction, ClearAction, ClearAllAction,
    MemClearAction, MemRecallAction, MemAddAction, MemSubAction, MemStoreAction,
    EnterDecimalAction>;

State reduce(State, Actions);

using Model = Ui::Model<State, Actions, reduce>;

} // namespace Calculator
