#include <skift/logger.h>

int main(int argc, char **argv)
{
    UNUSED(argc); UNUSED(argv);

    logger_log(LOG_INFO, "Starting graphic device driver...");
    
    return 0;
}
