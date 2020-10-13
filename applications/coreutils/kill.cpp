#include <libsystem/io/Stream.h>
#include <libsystem/process/Process.h>
#include <libsystem/utils/NumberParser.h>

int kill(const char *id)
{
    int i = parse_uint_inline(PARSER_DECIMAL, id, -1);

    if (i == -1)
    {
        return -1;
    }

    return process_cancel(i);
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        stream_format(err_stream, "%s: not enough arguments\n", argv[0]);
        return PROCESS_FAILURE;
    }

    int result = 0;

    for (int i = 1; i < argc; i++)
    {
        result |= kill(argv[i]);
    }

    return result;
}
