#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#define O_RDONLY (1 << 0)
#define O_WRONLY (1 << 1)
#define O_RDWR   (1 << 2)

#define O_CREAT  (1 << 3)
#define O_APPEND (1 << 4)
#define O_TRUNC  (1 << 5)

int  open(const char * path, int oflags, ...);