/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/Result.h>
#include <libsystem/io/Stream.h>
#include <libsystem/logger.h>
#include <libsystem/process/Process.h>

#define RESULT_ENUM_ENTRY_STRING(__entry) #__entry,
#define RESULT_ENUM_ENTRY_STRING_WITH_VALUE(__entry, __value) #__entry,

static Result error_value = SUCCESS;

const char *ERROR_NAMES[] = {RESULT_ENUM(RESULT_ENUM_ENTRY_STRING, RESULT_ENUM_ENTRY_STRING_WITH_VALUE)};

const char *result_to_string(Result error)
{
    return ERROR_NAMES[error];
}

Result error_get(void)
{
    return error_value;
}

void error_set(Result error)
{
    error_value = error;
}

void error_print(const char *message)
{
    logger_error("%s: %s", message, result_to_string(error_value));
    stream_printf(err_stream, "%s: %s\n", message, result_to_string(error_value));
}