#include <libsystem/core/Plugs.h>
#include <stdio.h>

void __plug_assert_failed(const char *expr, const char *file, const char *function, int line)
{
    fprintf(stdlog, "Assert failed: %s in %s:%s() ln%d!", expr, file, function, line);
    abort();
}