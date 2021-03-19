#pragma once

#include <assert.h>
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

    T value_or_default(T default_)
    {
        if (success())
        {
            return _value.value();
        }
        else
        {
            return default_;
        }
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
};

static inline Result __extract_result(Result r) { return r; }

template <typename T>
static inline Result __extract_result(ResultOr<T> r) { return r.result(); };

static inline Result __extract_value(Result r) { return r; }

template <typename T>
static inline T __extract_value(ResultOr<T> r) { return r.value(); };

// This macro works like the try!() macro from rust.
// If __stuff evaluate to an error it will short-circuit the function returning the error.
// Else __stuff is SUCCESS then propagate the value.
#define TRY(__stuff)                               \
    ({                                             \
        auto __eval__ = __stuff;                   \
                                                   \
        if (__extract_result(__eval__) != SUCCESS) \
        {                                          \
            return __extract_result(__eval__);     \
        }                                          \
                                                   \
        __extract_value(__eval__);                 \
    })
