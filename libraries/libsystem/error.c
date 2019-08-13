/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/error.h>
#include <libsystem/iostream.h>
#include <libsystem/process.h>

#define ERROR_ENUM_ENTRY_STRING(__entry) #__entry,
#define ERROR_ENUM_ENTRY_STRING_WITH_VALUE(__entry, __value) #__entry,

static error_t error_value = ERR_SUCCESS;

const char *ERROR_NAMES[] = {ERROR_ENUM(ERROR_ENUM_ENTRY_STRING, ERROR_ENUM_ENTRY_STRING_WITH_VALUE)};

const char *error_to_string(error_t error)
{
    return ERROR_NAMES[error];
}

error_t error_get(void)
{
    return error_value;
}

void error_set(error_t error)
{
    error_value = error;
}

void error_print(const char* message)
{
    iostream_printf(err_stream, "%s: %s\n", message, error_to_string(error_value));
}
 
void if_error_throw_and_catch_fire(const char* message)
{
    if (error_value != ERR_SUCCESS)
    {
        error_print(message);
        process_exit(-1);
    }
}