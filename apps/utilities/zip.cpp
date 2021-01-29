
#include <libfile/ZipArchive.h>
#include <libsystem/io/File.h>
#include <libsystem/io/Stream.h>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        stream_format(err_stream, "%s: Missing archive operand\n", argv[0]);
        return PROCESS_FAILURE;
    }

    if (argc == 2)
    {
        stream_format(err_stream, "%s: Missing files to pack into archive '%s'\n", argv[0], argv[1]);
        return PROCESS_FAILURE;
    }

    File file{argv[1]};
    if (file.exist())
    {
        stream_format(err_stream, "%s: Destination archive already exists '%s'\n", argv[0], argv[1]);
        return PROCESS_FAILURE;
    }

    ZipArchive archive(file, false);

    // Pack all files that were passed as arguments
    for (int i = 2; i < argc; i++)
    {
        printf("%s: Entry: %s is being inserted...\n", argv[0], argv[i]);
        auto result = archive.insert(argv[i], argv[i]);
        if (result != Result::SUCCESS)
        {
            stream_format(err_stream, "%s: Failed to insert entry '%s' with error '%i'", argv[0], argv[i], result);
        }
    }

    return PROCESS_SUCCESS;
}
