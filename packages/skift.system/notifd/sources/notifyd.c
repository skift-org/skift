#include <skift/logger.h>

int main(int argc, char* argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    sk_log(LOG_FINE, __PACKAGE__);

    return 0;
}