
#include <libsystem/Result.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Process.h>
#include <libsystem/Logger.h>

#define RESULT_ENUM_ENTRY_STRING(__entry) #__entry,
#define RESULT_ENUM_ENTRY_STRING_WITH_VALUE(__entry, __value) #__entry,

const char *RESULT_NAMES[] = {RESULT_ENUM(RESULT_ENUM_ENTRY_STRING, RESULT_ENUM_ENTRY_STRING_WITH_VALUE)};

const char *result_to_string(Result error)
{
    if (error < __RESULT_COUNT && error >= 0)
    {
        return RESULT_NAMES[error];
    }
    else
    {
        return "INVALID_RESULT_CODE";
    }
}
