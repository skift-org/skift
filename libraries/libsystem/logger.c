/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/cstring.h>

#include <libsystem/__plugs__.h>
#include <libsystem/io/Stream.h>
#include <libsystem/logger.h>
#include <libsystem/process/Process.h>

static bool logger_log_level = LOGGER_TRACE;

static bool logger_use_colors = true;
static bool logger_is_quiet = false;

static const char *logger_level_names[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
static const char *logger_level_colors[] = {"\e[34m", "\e[36m", "\e[32m", "\e[33m", "\e[31m", "\e[35m"};

void logger_level(LogLevel log_level)
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

void logger_log(LogLevel level, const char *file, uint line, const char *fmt, ...)
{
    if (level >= logger_log_level)
    {
        __plug_logger_lock();

        if (process_this() >= 0)
        {
            stream_printf(log_stream, "%3d: ", process_this());
        }
        else
        {
            stream_printf(log_stream, "     ", process_this());
        }

        DateTime datetime = datetime_now();
        stream_printf(log_stream, "%02d:%02d:%02d ", datetime.hour, datetime.minute, datetime.second);

        if (logger_use_colors)
        {
            stream_printf(log_stream, "%s%s \e[0m%s:%d: \e[37;1m", logger_level_colors[level], logger_level_names[level], file, line);
        }
        else
        {
            stream_printf(log_stream, "%s %s:%d: ", logger_level_names[level], file, line);
        }

        va_list va;
        va_start(va, fmt);

        stream_vprintf(log_stream, fmt, va);
        stream_printf(log_stream, "\e[0m\n");
        stream_flush(log_stream);

        va_end(va);

        __plug_logger_unlock();
    }
}
