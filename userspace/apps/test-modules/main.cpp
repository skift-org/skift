import TestModules;

#include <libsystem/Logger.h>

int main(int, char const *[])
{
    logger_trace("test %s", test());
    return 0;
}
