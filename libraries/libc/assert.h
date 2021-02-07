#pragma once

#include <__libc__.h>

#include <libsystem/Common.h>

__BEGIN_HEADER

void __no_return assert_failed(const char *expr, const char *file, const char *function, int line);

#ifndef assert
#    define assert(__expr) ((__expr) ? (void)(0) : assert_failed(#    __expr, __FILE__, __FUNCTION__, __LINE__))
#endif

#define ASSERT_NOT_REACHED() \
    assert_failed("ASSERT_NOT_REACHED() reached!", __FILE__, __FUNCTION__, __LINE__)

#ifndef __cplusplus
#    define static_assert(__expr, __message) _Static_assert((__expr), __message)
#endif

__END_HEADER
