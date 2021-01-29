#pragma once

#include <libsystem/Assert.h>
#include <libsystem/Result.h>

#include <libutils/Move.h>
#include <libutils/Optional.h>

template <typename T>
class ResultOr
{
private:
    Result _result = SUCCESS;
    Optional<T> _value;

public:
    bool success() { return _result == SUCCESS; }

    T &value()
    {
        assert(success());

        return *_value;
    }

    const T &value() const
    {
        assert(success());

        return *_value;
    }

    T take_value()
    {
        assert(success());

        return _value.take_value();
    }

    Result result() const { return _result; }

    const char *description()
    {
        return get_result_description(_result);
    }

    ResultOr(Result result) : _result{result}, _value{} {}

    ResultOr(T value) : _result{SUCCESS}, _value{move(value)} {}

    bool operator==(Result result) const
    {
        return _result == result;
    }

    bool operator==(const T &other) const
    {
        return _value == other;
    }

    operator bool() const { return _result == SUCCESS; }

    T &operator*() { return value(); }

    const T &operator*() const { return value(); }

    T *operator->() { return &value(); }

    const T *operator->() const { return &value(); }
};
