#include <skift/logger.h>

int main(int argc, char **argv)
{
    UNUSED(argc); UNUSED(argv);

    logger_log(LOG_INFO, "Starting graphic mouse driver...");

    return 0;
}
