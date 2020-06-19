#pragma once

#include <libsystem/__plugs__.h>

#define assert(__expr) ((__expr) ? (void)(0) : __plug_assert_failed(#__expr, __FILE__, __FUNCTION__, __LINE__))

#define ASSERT_NOT_REACHED() \
    __plug_assert_failed("ASSERT_NOT_REACHED() reached!", __FILE__, __FUNCTION__, __LINE__)

#ifndef __cplusplus

#define static_assert(__expr, __message) _Static_assert((__expr), __message)

#endif
