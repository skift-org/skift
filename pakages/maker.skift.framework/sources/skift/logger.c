#include <stdio.h>
#include <stdarg.h>

#include <skift/__plugs.h>
#include <skift/logger.h>

log_level_t log_level = LOG_ALL;

struct
{
    uint level;
    const char *name;
} level_names[] =
{
    { LOG_FATAL, "fatal"     },
    { LOG_SEVERE, "severe"   },
    { LOG_ERROR, "error"     },
    { LOG_WARNING, "warning" },
    { LOG_INFO, "info"       },
    { LOG_CONFIG, "config"   },
    { LOG_DEBUG, "debug"     },
    { LOG_FINE, "fine"       }
};

const char *log_describe(log_level_t level)
{
    for (uint i = 0; i < ARRAY_SIZE(level_names); i++)
    {
        if (level >= level_names[i].level)
        {
            return level_names[i].name;
        }
    }

    return "all";
}

void sk_logger_log(log_level_t level, const char * file, uint line, const char * function, const char * fmt, ...)
{
    __plug_logger_lock();

    va_list va;
    va_start(va, fmt);

    printf("%s:%d: %s: In function %s:", file, line, log_describe(level), function);
    vprintf(fmt, va);

    va_end(va);

    __plug_logger_unlock();
}