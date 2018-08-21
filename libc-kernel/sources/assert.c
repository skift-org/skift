#include <assert.h>
#include "kernel/logging.h"

void __assert_failed(const char * expr, const char * file, const char * function, int line)
{
    __panic((char *)file, (char *)function, line, NULL, "Kernel assert failed: %s !", (char *)expr);
}
