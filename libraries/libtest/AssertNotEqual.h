#pragma once
#include <assert.h>
#include <libio/Format.h>

namespace Test
{
template <class A, class B>
inline void _assert_not_equal(const A a, const B b, const char *file, const char *function, int line)
{
    if constexpr (!(__CONFIG_IS_RELEASE__))
    {
        if (a == static_cast<A>(b))
        {
            assert_failed(IO::format("{} equals {}", a, b).cstring(), file, function, line);
        }
    }
}

} // namespace Test

#ifndef assert_not_equal
#    define assert_not_equal(_a, _b) Test::_assert_not_equal(_a, _b, __FILE__, __FUNCTION__, __LINE__)
#endif