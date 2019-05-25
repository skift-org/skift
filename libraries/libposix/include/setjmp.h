#pragma once

/* i386 */
#define JMP_BUF_SIZE 9

typedef int jmp_buf[JMP_BUF_SIZE];

extern void longjmp(jmp_buf j, int r);
extern int setjmp(jmp_buf j);