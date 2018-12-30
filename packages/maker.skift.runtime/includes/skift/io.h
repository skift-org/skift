#pragma once

#include <skift/types.h>
#include <skift/utils.h>
#include <skift/syscall.h>
#include "kernel/protocol.h"

DECL_SYSCALL1(sk_io_print, const char * msg);
DECL_SYSCALL2(sk_io_read, char * buffer, int size);

DECL_SYSCALL1(sk_io_mouse_get_state, mouse_state_t* state);
DECL_SYSCALL1(sk_io_mouse_set_state, mouse_state_t* state);

DECL_SYSCALL1(sk_io_graphic_blit, unsigned int *buffer);
DECL_SYSCALL2(sk_io_graphic_size, unsigned int *width, unsigned int *height);
DECL_SYSCALL5(sk_io_graphic_blit_region, unsigned int * buffer, unsigned int x, unsigned int y, unsigned int w, unsigned int h);
