#pragma once

#include <math.h>

#include <libasync/Observable.h>

namespace Calculator
{

enum struct Operation
{
    NONE,
    ADD,
    SUBSTRACT,
    MULTIPLY,
    DIVIDE,
    RECIPROCAL,
    SQRT,
    POWER,
};

struct Engine : public Async::Observable<Engine>
{
private:
    Operation _operation = Operation::NONE;
    double _accumulator = 0.0;
    double _working = 0.0;
    bool _working_empty = true;

    void do_operator()
    {
        if (_working_empty)
        {
            return;
        }

        if (_operation == Operation::NONE)
        {
            _accumulator = _working;
        }
        else if (_operation == Operation::ADD)
        {
            _accumulator += _working;
        }
        else if (_operation == Operation::SUBSTRACT)
        {
            _accumulator -= _working;
        }
        else if (_operation == Operation::MULTIPLY)
        {
            _accumulator *= _working;
        }
        else if (_operation == Operation::DIVIDE)
        {
            _accumulator /= _working;
        }
        else if (_operation == Operation::RECIPROCAL)
        {
            if (_accumulator == 0)
            {
                return;
            }
            _accumulator = 1 / _accumulator;
        }
        else if (_operation == Operation::SQRT)
        {
            _accumulator = sqrt(_accumulator);
        }
        else if (_operation == Operation::POWER)
        {
            _accumulator = pow(_accumulator, _working);
        }
    }

public:
    Operation operation() { return _operation; }

    double screen()
    {
        if (_working_empty)
        {
            return _accumulator;
        }
        else
        {
            return _working;
        }
    }

    Engine()
    {
    }

    void clear()
    {
        _working = 0;

        did_update();
    }

    void clear_all()
    {
        _accumulator = 0;
        _working = 0;
        _working_empty = true;
        _operation = Operation::NONE;

        did_update();
    }

    void equal()
    {
        do_operator();

        _working = 0;
        _working_empty = true;

        did_update();
    }

    void enter_number(int number)
    {
        _working *= 10;
        _working += number;
        _working_empty = false;

        did_update();
    }

    void enter_operation(Operation op)
    {
        do_operator();

        _working = 0;
        _working_empty = false;

        _operation = op;

        did_update();
    }
};

} // namespace Calculator
