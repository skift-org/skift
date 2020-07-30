#include <stdlib.h>

#include <libsystem/process/Process.h>

void abort()
{
    process_exit(-1);
}
