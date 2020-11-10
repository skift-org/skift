#include <libsystem/Assert.h>
#include <libsystem/core/Plugs.h>

void assert_failed(const char *expr, const char *file, const char *function, int line)
{
    __plug_assert_failed(expr, file, function, line);
}
