#include "model.h"

namespace Calculator {

State doOperator(State s, Operator op) {
    if (not s.hasRhs) {
        s.op = op;
        return s;
    }

    switch (op) {
    case Operator::NONE:
        s.lhs = s.rhs;
        return s;

    case Operator::ADD:
        s.lhs = s.lhs + s.rhs;
        return s;

    case Operator::SUB:
        s.lhs = s.lhs - s.rhs;
        return s;

    case Operator::MULT:
        s.lhs = s.lhs * s.rhs;
        return s;

    case Operator::DIV:
        s.lhs = s.lhs / s.rhs;
        return s;

    case Operator::MOD:
        s.lhs = s.lhs % s.rhs;
        return s;

    case Operator::SQUARE:
        s.lhs = s.rhs;
        s.rhs = s.rhs * s.rhs;
        s.op = Operator::NONE;
        return s;

    case Operator::SQRT:
        s.lhs = s.rhs;
        s.rhs = sqrt(s.lhs);
        s.op = Operator::NONE;
        return s;

    case Operator::INVERT_SIGN:
        s.lhs = s.rhs;
        s.rhs = -s.lhs;
        s.op = Operator::NONE;
        return s;

    case Operator::TO_PERCENT:
        s.lhs = s.rhs;
        s.rhs = s.lhs / 100;
        s.op = Operator::NONE;
        return s;

    case Operator::RESIPROCAL:
        s.lhs = s.rhs;
        s.rhs = 1 / s.lhs;
        s.op = Operator::NONE;
        return s;
    }
}

State reduce(State s, Actions action) {
    return action.visit(Visitor{
        [&](Operator op) {
            if (not isUnary(s.op))
                s = doOperator(s, s.op);
            if (isUnary(op)) {
                if ((not isUnary(s.op) and s.op != Operator::NONE))
                    s.rhs = s.lhs;
                s = doOperator(s, op);
            }

            s.op = op;
            s.rhs = isUnary(op) ? s.rhs : 0;
            s.hasRhs = true;

            return s;
        },
        [&](Number n) {
            s.rhs *= 10;
            s.rhs += n;
            s.hasRhs = true;

            return s;
        },
        [&](BackspaceAction) {
            s.rhs /= 10;
            return s;
        },
        [&](EqualAction) {
            s = doOperator(s, s.op);
            s.rhs = std::exchange(s.lhs, 0);
            s.hasRhs = true;
            s.op = Operator::NONE;
            return s;
        },
        [&](ClearAction) {
            s.rhs = 0;
            return s;
        },
        [&](ClearAllAction) {
            s.lhs = 0;
            s.rhs = 0;
            s.hasRhs = false;
            s.op = Operator::NONE;
            return s;
        },
        [&](MemClearAction) {
            s.hasMem = false;
            s.mem = 0;
            return s;
        },
        [&](MemRecallAction) {
            s.rhs = s.mem;
            s.hasRhs = true;
            return s;
        },
        [&](MemAddAction) {
            if (not s.hasMem) {
                s = reduce(s, MemStoreAction{});
            } else {
                s.mem += s.hasRhs ? s.rhs : s.lhs;
            }
            return s;
        },
        [&](MemSubAction) {
            if (not s.hasMem) {
                s = reduce(s, MemStoreAction{});
                s.mem = -s.mem;
            } else {
                s.mem -= s.hasRhs ? s.rhs : s.lhs;
            }
            return s;
        },
        [&](MemStoreAction) {
            s.mem = s.hasRhs ? s.rhs : s.lhs;
            s.hasMem = true;
            return s;
        },
        [&](EnterDecimalAction) {
            return s;
        },
    });
}

} // namespace Calculator
