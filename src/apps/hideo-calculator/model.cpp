#include "model.h"

namespace Hideo::Calculator {

void doOperator(State& s, Operator op) {
    if (not s.hasRhs)
        s.op = op;

    switch (op) {
    case Operator::NONE:
        s.lhs = s.rhs;
        break;

    case Operator::ADD:
        s.lhs = s.lhs + s.rhs;
        break;

    case Operator::SUB:
        s.lhs = s.lhs - s.rhs;
        break;

    case Operator::MULT:
        s.lhs = s.lhs * s.rhs;
        break;

    case Operator::DIV:
        if (s.rhs == 0) {
            s.error = "division by zero"s;
            break;
        }
        s.lhs = s.lhs / s.rhs;
        break;

    case Operator::MOD:
        s.lhs = s.lhs % s.rhs;
        break;

    case Operator::SQUARE:
        s.lhs = s.rhs;
        s.rhs = s.rhs * s.rhs;
        s.op = Operator::NONE;
        break;

    case Operator::SQRT:
        s.lhs = s.rhs;
        s.rhs = sqrt(s.lhs);
        s.op = Operator::NONE;
        break;

    case Operator::INVERT_SIGN:
        s.lhs = s.rhs;
        s.rhs = -s.lhs;
        s.op = Operator::NONE;
        break;

    case Operator::TO_PERCENT:
        s.lhs = s.rhs;
        s.rhs = s.lhs / 100;
        s.op = Operator::NONE;
        break;

    case Operator::RESIPROCAL:
        s.lhs = s.rhs;
        s.rhs = 1 / s.lhs;
        s.op = Operator::NONE;
        break;
    }
}

Ui::Task<Action> reduce(State& s, Action action) {
    s.error = NONE;

    action.visit(Visitor{
        [&](Operator op) {
            if (not isUnary(s.op))
                doOperator(s, s.op);
            if (isUnary(op)) {
                if ((not isUnary(s.op) and s.op != Operator::NONE))
                    s.rhs = s.lhs;
                doOperator(s, op);
            }

            s.op = op;
            s.rhs = isUnary(op) ? s.rhs : 0;
            s.hasRhs = true;
        },
        [&](Number n) {
            s.rhs *= 10;
            s.rhs += n;
            s.hasRhs = true;
        },
        [&](BackspaceAction) {
            s.rhs /= 10;
        },
        [&](EqualAction) {
            doOperator(s, s.op);
            s.rhs = std::exchange(s.lhs, 0);
            s.hasRhs = true;
            s.op = Operator::NONE;
        },
        [&](ClearAction) {
            s.rhs = 0;
        },
        [&](ClearAllAction) {
            s.lhs = 0;
            s.rhs = 0;
            s.hasRhs = false;
            s.op = Operator::NONE;
        },
        [&](MemClearAction) {
            s.hasMem = false;
            s.mem = 0;
        },
        [&](MemRecallAction) {
            s.rhs = s.mem;
            s.hasRhs = true;
        },
        [&](MemAddAction) {
            if (not s.hasMem) {
                reduce(s, MemStoreAction{}).unwrap();
            } else {
                s.mem += s.hasRhs ? s.rhs : s.lhs;
            }
        },
        [&](MemSubAction) {
            if (not s.hasMem) {
                reduce(s, MemStoreAction{}).unwrap();
                s.mem = -s.mem;
            } else {
                s.mem -= s.hasRhs ? s.rhs : s.lhs;
            }
        },
        [&](MemStoreAction) {
            s.mem = s.hasRhs ? s.rhs : s.lhs;
            s.hasMem = true;
        },
        [&](EnterDecimalAction) {
        },
    });

    return NONE;
}

} // namespace Hideo::Calculator
