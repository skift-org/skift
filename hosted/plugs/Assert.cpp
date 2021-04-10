#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void assert_failed(const char *expr, const char *file, const char *function, int line)
{
    printf("Assert failed: %s %s %s %d\n", expr, file, function, line);
    abort();
}
