#include <stdlib.h>

#include <libsystem/core/CString.h>
#include <libsystem/process/Process.h>
#include <libsystem/utils/NumberParser.h>

void abort()
{
    process_abort();
}

int atoi(const char *nptr)
{
    int value;
    parse_int(PARSER_DECIMAL, nptr, strlen(nptr), &value);
    return value;
}

long int strtol(const char *nptr, char **endptr, int base)
{
    int value;
    parse_int({base}, nptr, (uintptr_t)endptr - (uintptr_t)nptr, &value);
    return value;
}

long long int strtoll(const char *nptr, char **endptr, int base)
{
    int value;
    parse_int({base}, nptr, (uintptr_t)endptr - (uintptr_t)nptr, &value);
    return value;
}
