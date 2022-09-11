#include "model.h"

namespace Calculator {

State doOperator(State s, Operator op) {
    if (!s.hasB)
        return s;

    switch (op) {
    case Operator::NONE:
        s.regA = s.regB;
        return s;

    case Operator::ADD:
        s.regA += s.regB;
        return s;

    case Operator::SUB:
        s.regA -= s.regB;
        return s;

    case Operator::MULT:
        s.regA *= s.regB;
        return s;

    case Operator::DIV:
        s.regA /= s.regB;
        return s;

    case Operator::MOD:
        s.regA %= s.regB;
        return s;

    case Operator::SQRT:
        s.regA = sqrt(s.regA);
        return s;

    case Operator::POWER:
        s.regA = pow(s.regA, s.regB);
        return s;
    }
}

State reduce(State s, Actions action) {
    return action.visit(Visitor{
        [&](Operator op) {
            s = doOperator(s, op);
            s.lastOp = op;
            s.regB = 0;
            s.hasB = false;

            return s;
        },
        [&](Number n) {
            s.regB *= 10;
            s.regB += n;
            s.hasB = true;

            return s;
        },
        [&](EqualAction) {
            s = doOperator(s, s.lastOp);
            s.regB = 0;
            s.hasB = false;

            return s;
        },
        [&](ClearAction) {
            s.regB = 0;

            return s;
        },
        [&](ClearAllAction) {
            s.lastOp = Operator::NONE;
            s.regA = 0;
            s.regB = 0;
            s.hasB = false;

            return s;
        },
    });
}

} // namespace Calculator
