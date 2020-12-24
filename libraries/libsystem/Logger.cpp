
#include <libsystem/core/CString.h>

#include <libsystem/Logger.h>
#include <libsystem/core/Plugs.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Process.h>

static bool logger_log_level = LOGGER_TRACE;

static bool logger_is_quiet = false;

static const char *logger_level_colors[] = {"\e[34m", "\e[36m", "\e[32m", "\e[33m", "\e[31m", "\e[35m"};

void logger_level(LogLevel log_level)
{
    logger_log_level = log_level;
}

void logger_quiet(bool quiet)
{
    logger_is_quiet = quiet;
}

void logger_log(LogLevel level, const char *file, int line, const char *fmt, ...)
{
    if (level >= logger_log_level)
    {
        __plug_logger_lock();

#ifndef __KERNEL__
        stream_format(log_stream, "\e[1m");
#endif

        if (process_this() >= 0)
        {
            stream_format(log_stream, "%3d: ", process_this());
        }
        else
        {
            stream_format(log_stream, "     ", process_this());
        }

        stream_format(log_stream, "%s: ", process_name());

#ifndef __KERNEL__
        stream_format(log_stream, "\e[m");
#endif

        DateTime datetime = datetime_now();
        stream_format(log_stream, "%02d:%02d:%02d ", datetime.hour, datetime.minute, datetime.second);

        stream_format(log_stream, "%s%s:%d:\e[37;1m ", logger_level_colors[level], file, line);

        va_list va;
        va_start(va, fmt);

        stream_vprintf(log_stream, fmt, va);
        stream_format(log_stream, "\e[0m\n");
        stream_flush(log_stream);

        va_end(va);

        if (level == LOGGER_FATAL)
        {
            __plug_logger_fatal();
        }

        __plug_logger_unlock();
    }
}
