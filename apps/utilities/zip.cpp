
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

    File file{args.argv()[0]};
    if (file.exist())
    {
        stream_format(err_stream, "%s: Destination archive already exists '%s'\n", argv[0], args.argv()[0].cstring());
        return PROCESS_FAILURE;
    }

    auto archive = make<ZipArchive>(Path::parse(args.argv()[0]), false);

    // Pack all files that were passed as arguments
    for (unsigned int i = 1; i < args.argc(); i++)
    {
        printf("%s: Entry: %s is being inserted...\n", argv[0], args.argv()[i].cstring());
        auto result = archive->insert(args.argv()[i].cstring(), args.argv()[i].cstring());
        if (result != Result::SUCCESS)
        {
            stream_format(err_stream, "%s: Failed to insert entry '%s' with error '%s'", argv[0], args.argv()[i].cstring(), get_result_description(result));
            return PROCESS_FAILURE;
        }
    }

    return PROCESS_SUCCESS;
}
