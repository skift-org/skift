#include <string.h>
#include <math.h>

#include <libsystem/runtime.h>
#include <libsystem/logger.h>

int sscanf(const char *str, const char *format, ...)
{
    __unused(str);
    __unused(format);

    logger_trace("sscanf(\"%s\", ...)", format);
    
    return 0;
}