#pragma once

#include <karm-ui/reducer.h>

namespace Calculator {

enum struct Operator;

struct State {
    Operator lastOp;
    int regA;
    int regB;
    bool hasB;

    int onScreen() {
        return hasB ? regB : regA;
    }
};

enum struct Operator {
    NONE,

    ADD,
    SUB,
    MULT,
    DIV,
    MOD,
    SQRT,
    POWER,
};

using Number = int;

struct EqualAction {};

struct ClearAction {};

struct ClearAllAction {};

using Actions = Var<
    Operator, Number, EqualAction, ClearAction, ClearAllAction>;

State reduce(State, Actions);

using Model = Ui::Model<State, Actions>;

} // namespace Calculator
