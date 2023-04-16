#ifndef _FENV_H
#define _FENV_H

#include <stdint.h>

#if defined(__x86_64__)

#    define FE_DIVBYZERO 4
#    define FE_INEXACT 32
#    define FE_INVALID 1
#    define FE_OVERFLOW 8
#    define FE_UNDERFLOW 16

#    define FE_ALL_EXCEPT 63

#    define FE_TONEAREST 0
#    define FE_DOWNWARD 0x400
#    define FE_UPWARD 0x800
#    define FE_TOWARDZERO 0xC00

#elif defined(__aarch64__)

#    define FE_INVALID 1
#    define FE_DIVBYZERO 2
#    define FE_OVERFLOW 4
#    define FE_UNDERFLOW 8
#    define FE_INEXACT 16

#    define FE_ALL_EXCEPT 31

#    define FE_TONEAREST 0
#    define FE_UPWARD 0x400000
#    define FE_DOWNWARD 0x800000
#    define FE_TOWARDZERO 0xC00000

#elif defined(__riscv) && __riscv_xlen == 64

#    define FE_INEXACT 1
#    define FE_UNDERFLOW 2
#    define FE_OVERFLOW 4
#    define FE_DIVBYZERO 8
#    define FE_INVALID 16

#    define FE_ALL_EXCEPT 31

#    define FE_TONEAREST 0
#    define FE_TOWARDZERO 1
#    define FE_DOWNWARD 2
#    define FE_UPWARD 3

#else
#    error Unknown architecture
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
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
int fesetenv(fenv_t const *);
int fesetexceptflag(fexcept_t const *, int);
int fesetround(int);
int fetestexcept(int);
int feupdateenv(fenv_t const *);

#ifdef __cplusplus
}
#endif

#define FE_DFL_ENV ((const fenv_t *)-1)

#endif // _FENV_H
