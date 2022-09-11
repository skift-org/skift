#pragma once

#include <karm-ui/reducer.h>

namespace Calculator {

enum struct Operator;

struct State {
    Operator op;
    int lhs;
    int rhs;
    bool hasRhs;

    int mem;
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
    }
}

inline bool isUnary(Operator op) {
    switch (op) {
    case Operator::SQUARE:
    case Operator::SQRT:
    case Operator::INVERT_SIGN:
    case Operator::TO_PERCENT:
        return true;
    default:
        return false;
    }
}

using Number = int;

struct BackspaceAction {};

struct EqualAction {};

struct ClearAction {};

struct ClearAllAction {};

struct MemClearAction {};

struct MemRecallAction {};

struct MemAddAction {};

struct MemSubAction {};

struct MemStoreAction {};

using Actions = Var<
    Operator, Number, BackspaceAction, EqualAction, ClearAction, ClearAllAction,
    MemClearAction, MemRecallAction, MemAddAction, MemSubAction, MemStoreAction>;

State reduce(State, Actions);

using Model = Ui::Model<State, Actions>;

} // namespace Calculator
