#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <stdio.h>
#include <stdlib.h>

#ifndef __PACKAGE__
    #define __PACKAGE__ "(NULL)"
#endif

#ifndef __FILENAME__
    #define __FILENAME__ "(NULL)"
#endif

#ifndef __COMMIT__
    #define __COMMIT__ "(NULL)"
#endif

#define PACKED(x) x __attribute__((packed))
#define ALIGNED(x, align) x __attribute__((aligned(align)))
#define UNUSED(x) (void)(x)
#define MALLOC(type) ((type *)malloc(sizeof(type)))
#define STUB(a...) printf("STUB REACHED %s\n", __FUNCTION__, a)
#define ARRAY_SIZE(array)  (sizeof(array) / sizeof(array[0]))
#define FLAG(__i) (1 << (__i))