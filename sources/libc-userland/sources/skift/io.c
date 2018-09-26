/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/io.h>
#include <skift/syscall.h>

DEFN_SYSCALL1(sk_io_print, SYS_PRINT, const char *);
DEFN_SYSCALL2(sk_io_read, SYS_READ, char *, int);