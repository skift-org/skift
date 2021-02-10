#include <assert.h>
#include <stdlib.h>

#include <skift/Plugs.h>

static bool _nested = false;

void assert_failed(const char *expr, const char *file, const char *function, int line)
{
    __plug_assert_failed(expr, file, function, line);

    if (_nested)
    {
        // nested assert faillur...
        exit(-1);
    }
    else
    {
        _nested = true;
    }

    abort();
    __builtin_unreachable();
}
