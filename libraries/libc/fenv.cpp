#include <assert.h>
#include <fenv.h>

int feclearexcept(int)
{
    assert(!"Not implemented");
    __builtin_unreachable();
}

int fegetenv(fenv_t *)
{
    assert(!"Not implemented");
    __builtin_unreachable();
}

int fegetexceptflag(fexcept_t *, int)
{
    assert(!"Not implemented");
    __builtin_unreachable();
}

int fegetround(void)
{
    assert(!"Not implemented");
    __builtin_unreachable();
}

int feholdexcept(fenv_t *)
{
    assert(!"Not implemented");
    __builtin_unreachable();
}

int feraiseexcept(int)
{
    assert(!"Not implemented");
    __builtin_unreachable();
}

int fesetenv(const fenv_t *)
{
    assert(!"Not implemented");
    __builtin_unreachable();
}

int fesetexceptflag(const fexcept_t *, int)
{
    assert(!"Not implemented");
    __builtin_unreachable();
}

int fesetround(int)
{
    assert(!"Not implemented");
    __builtin_unreachable();
}

int fetestexcept(int)
{
    assert(!"Not implemented");
    __builtin_unreachable();
}

int feupdateenv(const fenv_t *)
{
    assert(!"Not implemented");
    __builtin_unreachable();
}