
#include <libsystem/io/Stream.h>

int main(int argc, char **argv)
{
    printf("argc=%d\n", argc);
    for (int i = 0; i < argc; i++)
    {
        printf("argv[%d]='%s'\n", i + 1, argv[i]);
    }

    return 0;
}
