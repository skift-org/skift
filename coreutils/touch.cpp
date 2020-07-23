
#include <libsystem/io/Stream.h>

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    for (int i = 1; i < argc; i++)
    {
        Stream *stream = stream_open(argv[i], OPEN_CREATE);
        stream_close(stream);
    }

    return 0;
}
