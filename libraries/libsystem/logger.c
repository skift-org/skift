/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/cstring.h>

#include <libsystem/__plugs__.h>
#include <libsystem/iostream.h>
#include <libsystem/logger.h>
#include <libsystem/process.h>

static bool logger_log_level = LOGGER_TRACE;

static bool logger_use_colors = true;
static bool logger_is_quiet = false;

static const char *logger_level_names[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
static const char *logger_level_colors[] = {"\e[34m", "\e[36m", "\e[32m", "\e[22m", "\e[31m", "\e[35m"};

void logger_level(log_level_t log_level)
{
    logger_log_level = log_level;
}

void logger_colors(bool use_colors)
{
    logger_use_colors = use_colors;
}

void logger_quiet(bool quiet)
{
    logger_is_quiet = quiet;
}

void logger_log(log_level_t level, const char *file, uint line, const char *fmt, ...)
{
    if (level >= logger_log_level)
    {
        __plug_logger_lock();

        va_list va;
        va_start(va, fmt);

        // TODO: print the timestamp

        if (logger_use_colors)
        {
            iostream_printf(log_stream, "%d: %s%s \e[0m%s:%d: \e[37;1m", process_this(), logger_level_colors[level], logger_level_names[level], file, line);
        }
        else
        {
            iostream_printf(log_stream, "%d: %s%s %s:%d: ", process_this(), logger_level_names[level], file, line);
        }

        iostream_vprintf(log_stream, fmt, va);
        iostream_printf(log_stream, "\033[0m\n");
        iostream_flush(log_stream);

        va_end(va);

        __plug_logger_unlock();
    }
}
