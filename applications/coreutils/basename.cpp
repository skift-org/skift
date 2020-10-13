#include <libsystem/core/CString.h>
#include <libsystem/io/Path.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Process.h>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        stream_format(err_stream, "usage: %s path\n", argv[0]);
        return PROCESS_FAILURE;
    }

    Path *path = path_create(argv[1]);
    const char *basename = path_filename(path);

    if (basename == nullptr)
    {
        if (strcmp(argv[1], "") == 0)
        {
            printf(".\n");
        }
        else
        {
            printf("/\n");
        }
    }
    else
    {
        printf("%s\n", basename);
    }

    return PROCESS_SUCCESS;
}