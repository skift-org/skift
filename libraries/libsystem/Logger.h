#pragma once

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

void logger_log(LogLevel level, const char *file, uint line, const char *fmt, ...);

#define logger_trace(__args...) logger_log(LOGGER_TRACE, __FILE__, __LINE__, __args)

#define logger_debug(__args...) logger_log(LOGGER_DEBUG, __FILE__, __LINE__, __args)

#define logger_info(__args...) logger_log(LOGGER_INFO, __FILE__, __LINE__, __args)

#define logger_warn(__args...) logger_log(LOGGER_WARN, __FILE__, __LINE__, __args)

#define logger_error(__args...) logger_log(LOGGER_ERROR, __FILE__, __LINE__, __args)

#define logger_fatal(__args...) logger_log(LOGGER_FATAL, __FILE__, __LINE__, __args)

#define TRACE_FUNCTION_BEGIN logger_trace("begin %s from %08x", __FUNCTION__, __builtin_return_address(0))

#define TRACE_FUNCTION_END logger_trace("end %s", __FUNCTION__)
