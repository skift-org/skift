
#include <libio/Copy.h>
#include <libio/File.h>
#include <libio/Streams.h>
#include <libsystem/Result.h>
#include <libutils/Array.h>

Result cat(const char *path)
{
    IO::File file{path, OPEN_READ};

    TRY(file.result());

    return IO::copy(file, IO::out());
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        /* TODO: stdin option */
        return PROCESS_SUCCESS;
    }

    Result result;
    int exit_code = PROCESS_SUCCESS;

    for (int i = 1; i < argc; i++)
    {
        result = cat(argv[i]);

        if (result != SUCCESS)
        {
            IO::errln("{}: {}: {}", argv[0], argv[i], get_result_description(result));
            exit_code = PROCESS_FAILURE;
        }
    }

    return exit_code;
}
