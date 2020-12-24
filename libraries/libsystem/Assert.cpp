#include <libsystem/Assert.h>
#include <libsystem/core/Plugs.h>

static bool _nested = false;

void assert_failed(const char *expr, const char *file, const char *function, int line)
{
    if (_nested)
    {
        // nested assert faillur...
        __plug_process_exit(-1);
    }
    else
    {
        _nested = true;
    }

    __plug_assert_failed(expr, file, function, line);
}
