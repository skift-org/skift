#pragma once
#include <assert.h>
#include <libio/Format.h>

namespace Test
{
template <class A>
inline void _assert_not_null(const A a, const char *file, const char *function, int line)
{
    if constexpr (!(__CONFIG_IS_RELEASE__))
    {
        if (a == nullptr)
        {
            assert_failed("variable is nullptr", file, function, line);
        }
    }
}

} // namespace Test

#ifndef assert_not_null
#    define assert_not_null(_a) Test::_assert_not_null(_a, __FILE__, __FUNCTION__, __LINE__)
#endif