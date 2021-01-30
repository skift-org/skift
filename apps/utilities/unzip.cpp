
#include <libfile/ZipArchive.h>
#include <libsystem/io/File.h>
#include <libsystem/io/Filesystem.h>
#include <libsystem/io/Stream.h>
#include <libutils/Path.h>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        stream_format(err_stream, "%s: Missing archive argument(s)\n", argv[0]);
        return PROCESS_FAILURE;
    }

    // Unzip all archives that were passed as arguments
    for (int i = 1; i < argc; i++)
    {
        File file{argv[i]};
        if (!file.exist())
        {
            stream_format(err_stream, "%s: File does not exist '%s'", argv[0], argv[i]);
        }

        ZipArchive archive(file);

        if (!archive.valid())
        {
            stream_format(err_stream, "%s: Failed to read zip archive '%s'", argv[0], argv[i]);
            return PROCESS_FAILURE;
        }

        for (auto& entry : archive.entries())
        {
            printf("%s: Entry: %s is being extracted...\n", argv[0], entry.name.cstring());

            auto result = archive.extract(i, entry.name.cstring());
            if (result != Result::SUCCESS)
            {
                stream_format(err_stream, "%s: Failed to extract entry '%s' with error '%i'", argv[0], entry.name.cstring(), result);
            }
        }
    }

    return PROCESS_SUCCESS;
}
