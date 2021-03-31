#pragma once

#include <assert.h>

#include <libsystem/Common.h>

enum LogLevel
{
    LOGGER_TRACE,
    LOGGER_DEBUG,
    LOGGER_INFO,
    LOGGER_WARN,
    LOGGER_ERROR,
    LOGGER_FATAL,
};

void logger_level(LogLevel log_level);

void logger_colors(bool use_colors);

void logger_quiet(bool quiet);

void logger_log(LogLevel level, const char *file, int line, const char *fmt, ...);

#ifdef DISABLE_LOGGER
#    define logger_trace(__args...)
#    define logger_debug(__args...)
#    define logger_info(__args...)
#    define logger_warn(__args...)
#    define logger_error(__args...)
#    define logger_fatal(__args...) ASSERT_NOT_REACHED()
#else
#    define logger_trace(__args...) logger_log(LOGGER_TRACE, __FILE__, __LINE__, __args)
#    define logger_debug(__args...) logger_log(LOGGER_DEBUG, __FILE__, __LINE__, __args)
#    define logger_info(__args...) logger_log(LOGGER_INFO, __FILE__, __LINE__, __args)
#    define logger_warn(__args...) logger_log(LOGGER_WARN, __FILE__, __LINE__, __args)
#    define logger_error(__args...) logger_log(LOGGER_ERROR, __FILE__, __LINE__, __args)

#    define logger_fatal(__args...) ({                        \
        logger_log(LOGGER_FATAL, __FILE__, __LINE__, __args); \
        ASSERT_NOT_REACHED();                                 \
    })
#endif