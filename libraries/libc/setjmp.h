#pragma once

#include <__libc__.h>

__BEGIN_HEADER

/* i386 */
#define _JBLEN 6

typedef int jmp_buf[_JBLEN];

__attribute__((noreturn)) void longjmp(jmp_buf j, int r);

__attribute__((returns_twice)) int setjmp(jmp_buf j);

__END_HEADER
