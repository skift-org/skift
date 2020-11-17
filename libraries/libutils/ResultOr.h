#pragma once

#include <libsystem/Assert.h>
#include <libsystem/Result.h>
#include <libutils/Move.h>

template <typename T>
class ResultOr
{
private:
    Result _result;
    T _value;

public:
    bool success() { return _result == SUCCESS; }

    T &value()
    {
        assert(success());
        return _value;
    }

    T &&take_value()
    {
        assert(success());
        return move(_value);
    }

    Result result() { return _result; }

    ResultOr(Result result) : _result(result), _value() {}

    ResultOr(T value) : _result(SUCCESS), _value(move(value)) {}

    ResultOr(Result result, T value) : _result(result), _value(move(value)) {}

    template <typename U>
    ResultOr(ResultOr<U> result, T value) : _result(result.result()), _value(move(value)) {}

    bool operator==(Result result)
    {
        return _result == result;
    }

    bool operator!=(Result result)
    {
        return _result != result;
    }

    bool operator==(T value)
    {
        return _value == value;
    }

    bool operator!=(T value)
    {
        return _value != value;
    }
};
