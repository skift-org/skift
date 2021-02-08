
#include <libfile/ZipArchive.h>
#include <libsystem/io/File.h>
#include <libsystem/io_new/Streams.h>
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
        System::errln("{}: Destination archive already exists '{}'", argv[0], args.argv()[0]);
        return PROCESS_FAILURE;
    }

    auto archive = make<ZipArchive>(Path::parse(args.argv()[0]), false);

    // Pack all files that were passed as arguments
    for (unsigned int i = 1; i < args.argc(); i++)
    {
        System::outln("{}: Entry: {} is being inserted...", argv[0], args.argv()[i]);

        auto result = archive->insert(args.argv()[i].cstring(), args.argv()[i].cstring());

        if (result != Result::SUCCESS)
        {
            System::errln("{}: Failed to insert entry '{}' with error '{}'", argv[0], args.argv()[i], get_result_description(result));
            return PROCESS_FAILURE;
        }
    }

    return PROCESS_SUCCESS;
}
