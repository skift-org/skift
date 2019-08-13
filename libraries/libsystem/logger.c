/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/cstring.h>

#include <libsystem/__plugs__.h>
#include <libsystem/iostream.h>
#include <libsystem/logger.h>
#include <libsystem/process.h>

log_level_t log_level = LOG_ALL;
bool show_file_name = true;

struct
{
    uint level;
    const char *name;
} level_names[] = {
    {LOG_FATAL, "\033[41;1m[ fatal  ]"},
    {LOG_SEVERE, "\033[41;1m[ severe ]"},
    {LOG_ERROR, "\033[41m[ error ]"},
    {LOG_WARNING, "\033[33m[ warn ]"},
    {LOG_INFO, "\033[37m[ info ]"},
    {LOG_CONFIG, "\033[30;1m[ conf ]"},
    {LOG_DEBUG, "\033[30;1m[ debug ]"},
    {LOG_FINE, "\033[30;1m[ fine ]"},
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

void logger_setlevel(log_level_t level)
{
    log_level = level;
}

void __logger_log(log_level_t level, const char *file, uint line, const char *function, const char *fmt, ...)
{
    if (level >= log_level)
    {
        __plug_logger_lock();

        va_list va;
        va_start(va, fmt);

        if (show_file_name)
        {
            iostream_printf(log_stream, "[ %d ] %s %s:%s() ln%d ", process_this(), log_describe(level), file, function, line);
        }
        else
        {
            iostream_printf(log_stream, "[ %d ] %s %s() ", process_this(), log_describe(level), function);
        }

        iostream_vprintf(log_stream, fmt, va);
        iostream_printf(log_stream, "\033[0m\n");
        iostream_flush(log_stream);

        va_end(va);

        __plug_logger_unlock();
    }
}
