
#include <libfile/ZipArchive.h>
#include <libsystem/io/File.h>
#include <libsystem/io/Stream.h>
#include <libutils/ArgParse.h>

int main(int argc, char const *argv[])
{
    ArgParse args;

    args.show_help_if_no_operand_given();
    args.should_abort_on_failure();

    args.usage("ARCHIVE FILES...");
    args.usage("OPTION... ARCHIVE FILES...");

    args.eval(argc, argv);

    if (args.argc() < 2)
    {
        args.help();
        args.fail();
    }

    File file{argv[1]};
    if (file.exist())
    {
        stream_format(err_stream, "%s: Destination archive already exists '%s'\n", argv[0], argv[1]);
        return PROCESS_FAILURE;
    }

    ZipArchive archive(file, false);

    // Pack all files that were passed as arguments
    for (unsigned int i = 1; i < args.argc(); i++)
    {
        printf("%s: Entry: %s is being inserted...\n", argv[0], args.argv()[i].cstring());
        auto result = archive.insert(args.argv()[i].cstring(), args.argv()[i].cstring());
        if (result != Result::SUCCESS)
        {
            stream_format(err_stream, "%s: Failed to insert entry '%s' with error '%i'", argv[0], args.argv()[i], result);
            return PROCESS_FAILURE;
        }
    }

    return PROCESS_SUCCESS;
}
