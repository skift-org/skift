#include <skift/error.h>
#include <skift/iostream.h>

#define ERROR_ENUM_STRING(__value) #__value,

static error_t* error = ERR_SUCESS;

const char *ERROR_NAMES[] = {ERROR_ENUM(ERROR_ENUM_STRING)};

const char *error_to_string(error_t error)
{
    return ERROR_NAMES[error];
}

error_t* error_get(void)
{
    return error;
}

void error_print(const char* message)
{
    printf("%s: %s\n", message, error_to_string(error));
}