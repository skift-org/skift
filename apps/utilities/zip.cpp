
#include <libfile/ZipArchive.h>
#include <libio/File.h>
#include <libio/Streams.h>
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

    IO::File file{args.argv()[0]};
    if (file.exist())
    {
        IO::errln("{}: Destination archive already exists '{}'", argv[0], args.argv()[0]);
        return PROCESS_FAILURE;
    }

    auto archive = make<ZipArchive>(Path::parse(args.argv()[0]), false);

    // Pack all files that were passed as arguments
    for (unsigned int i = 1; i < args.argc(); i++)
    {
        IO::outln("{}: Entry: {} is being inserted...", argv[0], args.argv()[i]);

        IO::File src_file(args.argv()[i], OPEN_READ);
        if (!src_file.exist())
        {
            IO::errln("{}: File '{}' does not exist", argv[0], args.argv()[i]);
            return PROCESS_FAILURE;
        }

        auto result = archive->insert(args.argv()[i].cstring(), src_file);

        if (result != Result::SUCCESS)
        {
            IO::errln("{}: Failed to insert entry '{}' with error '{}'", argv[0], args.argv()[i], get_result_description(result));
            return PROCESS_FAILURE;
        }
    }

    return PROCESS_SUCCESS;
}
