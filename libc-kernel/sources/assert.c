#ifdef __KERNEL

#include <assert.h>
#include "kernel/logging.h"

void __assert_failed(string expr, string file, string function, int line)
{
    __panic(file, function, line, NULL, "Kernel assert failed: %s !", expr);
}

#endif