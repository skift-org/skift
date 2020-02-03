#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/__plugs__.h>

#define assert(__expr)                                                   \
    if (!(__expr))                                                       \
    {                                                                    \
        __plug_assert_failed(#__expr, __FILE__, __FUNCTION__, __LINE__); \
    }

#define ASSERT_NOT_REACHED() \
    __plug_assert_failed("ASSERT_NOT_REACHED() reached!", __FILE__, __FUNCTION__, __LINE__)

#define static_assert(__expr, __message) _Static_assert((__expr), (__message))