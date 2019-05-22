#include <skift/error.h>
#include <skift/iostream.h>
#include <skift/process.h>

#define ERROR_ENUM_STRING(__value) #__value,

static error_t* error_value = ERR_SUCESS;

const char *ERROR_NAMES[] = {ERROR_ENUM(ERROR_ENUM_STRING)};

const char *error_to_string(error_t error)
{
    return ERROR_NAMES[error];
}

error_t* error_get(void)
{
    return error_value;
}

void error_print(const char* message)
{
    printf("%s: %s\n", message, error_to_string(error_value));
}
 
void if_error_throw_and_catch_fire(const char* message)
{
    if (error_value != ERR_SUCESS)
    {
        error_print(message);
        process_exit(-1);
    }
}