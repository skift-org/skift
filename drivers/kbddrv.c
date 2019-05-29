#include <skift/logger.h>

int main(int argc, char **argv)
{
    UNUSED(argc); UNUSED(argv);

    logger_log(LOG_INFO, "Starting keyboard device driver...");

    return 0;
}
