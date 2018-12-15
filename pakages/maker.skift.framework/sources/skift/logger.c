#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include <skift/__plugs.h>
#include <skift/logger.h>

log_level_t log_level = LOG_ALL;
bool show_file_name = false;

struct
{
    uint level;
    const char *name;
} level_names[] =
{
    { LOG_FATAL, "\033[31;1mfatal"   },
    { LOG_SEVERE, "\033[31msevere"   },
    { LOG_ERROR, "\033[31merror"     },
    { LOG_WARNING, "\033[33mwarning" },
    { LOG_INFO, "\033[34minfo"       },
    { LOG_CONFIG, "\033[37mconfig"   },
    { LOG_DEBUG, "\033[37mdebug"     },
    { LOG_FINE, "\033[37mfine"       }
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

void sk_logger_setlevel(log_level_t level)
{
    log_level = level;
}

char logbuffer[1024];

void sk_logger_log(log_level_t level, const char *file, uint line, const char *function, const char *fmt, ...)
{
    if (level >= log_level)
    {
        __plug_logger_lock();

        va_list va;
        va_start(va, fmt);

        char *buffer = logbuffer;
        buffer[0] = '\0';

        if (show_file_name)
        {
            buffer += sprintf(buffer, "%s:%d: %s: In function %s: ", file, line, log_describe(level), function);
        }
        else
        {
            buffer += sprintf(buffer, "%s: In function %s: ", log_describe(level), function);
        }

        buffer += vsprintf(buffer, fmt, va);
        strcat(buffer, "\033[0m\n");

        __plug_print(logbuffer);

        va_end(va);

        __plug_logger_unlock();
    }
}