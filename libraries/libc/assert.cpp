#include <assert.h>
#include <stdlib.h>

static bool _nested = false;

void assert_failed(const char *expr, const char *file, const char *function, int line)
{
    __unused(expr);
    __unused(file);
    __unused(function);
    __unused(line);

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
