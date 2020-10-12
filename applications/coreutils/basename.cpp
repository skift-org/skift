#include <libsystem/core/CString.h>
#include <libsystem/io/Path.h>
#include <libsystem/io/Stream.h>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        stream_format(err_stream, "usage: %s path\n", argv[0]);
        return -1;
    }

    Path *path = path_create(argv[1]);
    const char *basename = path_filename(path);

    if (basename == nullptr)
    {
        if (strcmp(argv[1], "") == 0)
        {
            printf(".");
        }
        else
        {
            printf("/");
        }
    }
    else
    {
        printf("%s", basename);
    }
    return 0;
}