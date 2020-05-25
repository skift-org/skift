#include <libsystem/logger.h>
#include <stdio.h>

__attribute__((constructor)) void test_ctor()
{
    logger_debug("Hello I'm the ctor :)");
}

int main(int argc, char const *argv[])
{
    (void)argc;
    (void)argv;

    for (size_t i = 0; i < 80; i++)
    {
        putchar('=');
    }

    return 0;
}
