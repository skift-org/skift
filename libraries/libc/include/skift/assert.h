#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/__plugs__.h>

#define assert(expr)                                                  \
    if (!(expr))                                                      \
    {                                                                 \
        __plug_assert_failed(#expr, __FILE__, __FUNCTION__, __LINE__); \
    }