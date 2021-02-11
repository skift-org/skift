#pragma once

#include <__libc__.h>
#include <stdint.h>

__BEGIN_HEADER

#define _FENV_H

#define FE_DIVBYZERO 4
#define FE_INEXACT 32
#define FE_INVALID 1
#define FE_OVERFLOW 8
#define FE_UNDERFLOW 16
#define __FE_DENORM 2

#define FE_ALL_EXCEPT 63

#define FE_DOWNWARD 0x400
#define FE_TONEAREST 0
#define FE_TOWARDZERO 0xC00
#define FE_UPWARD 0x800

typedef struct
{
    uint32_t __control_word;
    uint32_t __status_word;
    uint32_t __unused[5];
    uint32_t __mxcsr;
} fenv_t;

typedef uint16_t fexcept_t;

int feclearexcept(int);
int fegetenv(fenv_t *);
int fegetexceptflag(fexcept_t *, int);
int fegetround(void);
int feholdexcept(fenv_t *);
int feraiseexcept(int);
int fesetenv(const fenv_t *);
int fesetexceptflag(const fexcept_t *, int);
int fesetround(int);
int fetestexcept(int);
int feupdateenv(const fenv_t *);

#define FE_DFL_ENV ((const fenv_t *)-1)

__END_HEADER