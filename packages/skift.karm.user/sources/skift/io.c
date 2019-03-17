/* Copyright © 2018-2019 N. Van Bossuyt.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/io.h>

DEFN_SYSCALL1(sk_io_print, SYS_IO_PRINT, const char *);
DEFN_SYSCALL2(sk_io_read, SYS_IO_READ, char *, int);

DEFN_SYSCALL1(sk_io_graphic_blit, SYS_IO_GRAPHIC_BLIT, unsigned int *);
DEFN_SYSCALL5(sk_io_graphic_blit_region, SYS_IO_GRAPHIC_BLIT_REGION, unsigned int *, unsigned int, unsigned int, unsigned int, unsigned int);
DEFN_SYSCALL2(sk_io_graphic_size, SYS_IO_GRAPHIC_SIZE, unsigned int *, unsigned int *);