
#include <libsystem/io/Filesystem.h>
#include <libsystem/io/Stream.h>

int main(int argc, char **argv)
{
    if (argc == 3)
    {
        Result result = filesystem_rename(argv[1], argv[2]);
        if (result != SUCCESS)
        {
            stream_format(err_stream, "Failled to move file: %s", result_to_string(result));
            return -1;
        }
        else
        {
            return 0;
        }
    }

    return -1;
}
