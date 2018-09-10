#pragma once

#include <skift/syscall.h>

DECL_SYSCALL1(sk_io_print, const char * msg);
DECL_SYSCALL2(sk_io_read, char * buffer, int size);