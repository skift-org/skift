#pragma once

#include <libsystem/Common.h>

void __no_return assert_failed(const char *expr, const char *file, const char *function, int line);

#define assert(__expr) ((__expr) ? (void)(0) : assert_failed(#__expr, __FILE__, __FUNCTION__, __LINE__))

#define ASSERT_NOT_REACHED() \
    assert_failed("ASSERT_NOT_REACHED() reached!", __FILE__, __FUNCTION__, __LINE__)

#ifndef __cplusplus
#    define static_assert(__expr, __message) _Static_assert((__expr), __message)
#endif
