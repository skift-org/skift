#pragma once

#include <skift/syscall.h>

DECL_SYSCALL1(sk_io_print, const char * msg);
DECL_SYSCALL2(sk_io_read, char * buffer, int size);

DECL_SYSCALL2(sk_io_mouse_get_position, int *outxpos, int *outypos);
DECL_SYSCALL2(sk_io_mouse_set_position, int xpos, int ypos);

DECL_SYSCALL1(sk_io_graphic_blit, unsigned int *buffer);
DECL_SYSCALL2(sk_io_graphic_size, unsigned int *width, unsigned int *height);