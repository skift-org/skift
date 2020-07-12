#pragma once

/* i386 */
#define _JBLEN 9

typedef int jmp_buf[_JBLEN];

__attribute__((noreturn)) extern void longjmp(jmp_buf j, int r);

__attribute__((returns_twice)) extern int setjmp(jmp_buf j);
