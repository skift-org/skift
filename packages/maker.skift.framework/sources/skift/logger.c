#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include <skift/__plugs.h>
#include <skift/logger.h>

log_level_t log_level = LOG_WARNING;
bool show_file_name = true;

struct
{
    uint level;
    const char *name;
} level_names[] =
{
    { LOG_FATAL,   "\033[31;1m[ fatal  ]" },
    { LOG_SEVERE,  "\033[31;1m[ severe ]" },
    { LOG_ERROR,   "\033[31m[ error ]"    },
    { LOG_WARNING, "\033[33m[ warn ]"     },
    { LOG_INFO,    "\033[37m[ info ]"     },
    { LOG_CONFIG,  "\033[30;1m[ conf ]"   },
    { LOG_DEBUG,   "\033[30;1m[ debug ]"  },
    { LOG_FINE,    "\033[30;1m[ fine ]"   }
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
            buffer += sprintf(buffer, "%s %s:%s() ln%d ", log_describe(level), file, function, line);
        }
        else
        {
            buffer += sprintf(buffer, "%s %s() ", log_describe(level), function);
        }

        buffer += vsprintf(buffer, fmt, va);
        strcat(buffer, "\033[0m\n");

        __plug_print(logbuffer);

        va_end(va);

        __plug_logger_unlock();
    }
}