#include <libsystem/core/CString.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Process.h>
#include <libutils/Path.h>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        stream_format(err_stream, "usage: %s path\n", argv[0]);
        return PROCESS_FAILURE;
    }

    auto path = Path::parse(argv[1]);
    printf("%s\n", path.basename().cstring());

    return PROCESS_SUCCESS;
}
