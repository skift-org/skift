#include <assert.h>
#include "kernel/logger.h"

void __assert_failed(const char * expr, const char * file, const char * function, int line)
{
    log("Kernel assert failed: %s in %s:%s() ln%d!", (char *)expr, (char *)file, (char *)function, line);
}
