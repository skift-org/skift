#include <stdio.h>
#include <stdlib.h>

#include <libsystem/Assert.h>

void assert_failed(const char *expr, const char *file, const char *function, int line)
{
    fprintf(stderr, "Assert failed: %s in %s:%s() ln%d!\n", expr, file, function, line);

    abort();
}
