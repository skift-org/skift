#pragma once

#include <karm-ui/reducer.h>

namespace Hideo::Calculator {

enum struct Operator;

struct State {
    Operator op;
    isize lhs;
    isize rhs;
    bool hasRhs;
    isize mem;
    bool hasMem;
    Opt<Str> error;
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

using Action = Union<
    Operator, Number, BackspaceAction, EqualAction, ClearAction, ClearAllAction,
    MemClearAction, MemRecallAction, MemAddAction, MemSubAction, MemStoreAction,
    EnterDecimalAction>;

Ui::Task<Action> reduce(State &, Action);

using Model = Ui::Model<State, Action, reduce>;

} // namespace Hideo::Calculator
