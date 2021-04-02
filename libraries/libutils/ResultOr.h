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
    ALWAYS_INLINE bool success() { return _result == SUCCESS; }

    ALWAYS_INLINE T &unwrap()
    {
        assert(success());

        return _value.unwrap();
    }

    ALWAYS_INLINE const T &unwrap() const
    {
        assert(success());

        return _value.unwrap();
    }

    ALWAYS_INLINE T unwrap_or(T default_)
    {
        if (success())
        {
            return _value.unwrap();
        }
        else
        {
            return default_;
        }
    }

    ALWAYS_INLINE Result result() const { return _result; }

    ALWAYS_INLINE const char *description()
    {
        return get_result_description(_result);
    }

    ALWAYS_INLINE ResultOr(Result result) : _result{result}, _value{} {}

    ALWAYS_INLINE ResultOr(T value) : _result{SUCCESS}, _value{move(value)} {}
};

ALWAYS_INLINE static inline Result __extract_result(Result r) { return r; }

template <typename T>
ALWAYS_INLINE static inline Result __extract_result(ResultOr<T> r) { return r.result(); };

ALWAYS_INLINE static inline Result __extract_value(Result r) { return r; }

template <typename T>
ALWAYS_INLINE static inline T __extract_value(ResultOr<T> r) { return r.unwrap(); };

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
