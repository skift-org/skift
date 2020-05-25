
#include <libsystem/io/Stream.h>

int main(int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
        printf("%s ", argv);

    return 0;
}
