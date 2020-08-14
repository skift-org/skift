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

    T &&take_value()
    {
        assert(success());
        return move(_value);
    }

    Result result() { return _result; }

    ResultOr(Result result) : _result(result), _value() {}

    ResultOr(T value) : _result(SUCCESS), _value(value) {}

    ResultOr(Result result, T value) : _result(result), _value(value) {}

    template <typename U>
    ResultOr(ResultOr<U> result, T value) : _result(result.result()), _value(value) {}

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
