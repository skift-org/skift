#include <math.h>
#include <string.h>

#include <libsystem/Logger.h>

int sscanf(const char *str, const char *format, ...)
{
    __unused(str);
    __unused(format);

    logger_trace("sscanf(\"%s\", ...)", format);

    return 0;
}
