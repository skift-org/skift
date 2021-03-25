#include <libsystem/Result.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Process.h>

#define RESULT_ENUM_ENTRY_STRING(__entry, __description) #__entry,
#define RESULT_ENUM_ENTRY_STRING_WITH_VALUE(__entry, __value, __description) #__entry,

const char *RESULT_NAMES[] = {RESULT_ENUM(RESULT_ENUM_ENTRY_STRING, RESULT_ENUM_ENTRY_STRING_WITH_VALUE)};
const char *RESULT_DESCRIPTIONS[] = {
#define RESULT_ENUM_ENTRY(__name, __description) __description,
#define RESULT_ENUM_ENTRY_WITH_VALUE(__name, __value, __description) __description,
    RESULT_ENUM(RESULT_ENUM_ENTRY, RESULT_ENUM_ENTRY_WITH_VALUE)
#undef RESULT_ENUM_ENTRY
#undef RESULT_ENUM_ENTRY_WITH_VALUE
};

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

const char *get_result_description(Result result)
{
    if (result < __RESULT_COUNT && result >= 0)
    {
        return RESULT_DESCRIPTIONS[result];
    }
    else
    {
        return "INVALID_RESULT_CODE";
    }
}