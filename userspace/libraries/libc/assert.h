#pragma once

#include <__libc__.h>

#include <libutils/Prelude.h>

__BEGIN_HEADER

void NO_RETURN assert_failed(const char *expr, const char *file, const char *function, int line);

#ifndef assert
#    define assert(__expr) ((__expr) ? (void)(0) : assert_failed(#    __expr, __FILE__, __FUNCTION__, __LINE__))
#endif

#ifndef __cplusplus
#    define static_assert(__expr, __message) _Static_assert((__expr), __message)
#endif

#define ASSERT_NOT_REACHED() \
    assert("ASSERT_NOT_REACHED() reached!" && false)

__END_HEADER
