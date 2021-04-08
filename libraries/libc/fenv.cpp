#include <fenv.h>

#include <libutils/Assert.h>

int feclearexcept(int)
{
    ASSERT_NOT_REACHED();
}

int fegetenv(fenv_t *)
{
    ASSERT_NOT_REACHED();
}

int fegetexceptflag(fexcept_t *, int)
{
    ASSERT_NOT_REACHED();
}

int fegetround(void)
{
    ASSERT_NOT_REACHED();
}

int feholdexcept(fenv_t *)
{
    ASSERT_NOT_REACHED();
}

int feraiseexcept(int)
{
    ASSERT_NOT_REACHED();
}

int fesetenv(const fenv_t *)
{
    ASSERT_NOT_REACHED();
}

int fesetexceptflag(const fexcept_t *, int)
{
    ASSERT_NOT_REACHED();
}

int fesetround(int)
{
    ASSERT_NOT_REACHED();
}

int fetestexcept(int)
{
    ASSERT_NOT_REACHED();
}

int feupdateenv(const fenv_t *)
{
    ASSERT_NOT_REACHED();
}