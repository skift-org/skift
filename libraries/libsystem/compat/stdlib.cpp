#include <stdlib.h>

#include <libsystem/core/CString.h>
#include <libsystem/process/Process.h>
#include <libsystem/utils/NumberParser.h>

void abort()
{
    process_exit(-1);
}

int atoi(const char *nptr)
{
    int value;
    parse_int(PARSER_DECIMAL, nptr, strlen(nptr), &value);
    return value;
}
