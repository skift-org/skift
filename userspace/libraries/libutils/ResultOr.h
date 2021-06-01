#pragma once

#include <abi/Result.h>
#include <assert.h>

#include <libutils/Optional.h>
#include <libutils/Std.h>

namespace Utils
{

template <typename T>
struct ResultOr
{
private:
    HjResult _result = SUCCESS;
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

    ALWAYS_INLINE HjResult result() const { return _result; }

    ALWAYS_INLINE const char *description()
    {
        return get_result_description(_result);
    }

    ALWAYS_INLINE ResultOr(HjResult result) : _result{result}, _value{NONE} {}

    ALWAYS_INLINE ResultOr(T value) : _result{SUCCESS}, _value{std::move(value)} {}
};

ALWAYS_INLINE static inline HjResult __extract_result(HjResult r) { return r; }

template <typename T>
ALWAYS_INLINE static inline HjResult __extract_result(ResultOr<T> r) { return r.result(); };

ALWAYS_INLINE static inline HjResult __extract_value(HjResult r) { return r; }

template <typename T>
ALWAYS_INLINE static inline T __extract_value(ResultOr<T> r) { return r.unwrap(); };

// This macro works like the try!() macro from rust.
// If __stuff evaluate to an error it will short-circuit the function returning the error.
// Else __stuff is SUCCESS then propagate the value.
#define TRY(__stuff)                                        \
    ({                                                      \
        auto __eval__ = __stuff;                            \
                                                            \
        if (::Utils::__extract_result(__eval__) != SUCCESS) \
        {                                                   \
            return ::Utils::__extract_result(__eval__);     \
        }                                                   \
                                                            \
        ::Utils::__extract_value(__eval__);                 \
    })

} // namespace Utils