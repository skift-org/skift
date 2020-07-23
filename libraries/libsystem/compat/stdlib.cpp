#include <stdlib.h>

#include <libsystem/process/Process.h>

void abort(void)
{
    process_exit(-1);
}
