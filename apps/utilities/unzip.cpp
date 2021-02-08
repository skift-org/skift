
#include <libfile/ZipArchive.h>
#include <libsystem/io/File.h>
#include <libsystem/io/Filesystem.h>
#include <libsystem/io_new/Streams.h>
#include <libutils/ArgParse.h>
#include <libutils/Path.h>

int main(int argc, char const *argv[])
{
    ArgParse args;

    args.show_help_if_no_operand_given();
    args.should_abort_on_failure();

    args.usage("FILES...");
    args.usage("OPTION... FILES...");

    args.eval(argc, argv);

    // Unzip all archives that were passed as arguments
    args.argv().foreach ([&](auto &path) {
        System::errln("{}: Unzip '{}'", argv[0], path);

        File file{path};
        if (!file.exist())
        {
            System::errln("{}: File does not exist '{}'", argv[0], path);
            process_exit(PROCESS_FAILURE);
            return Iteration::STOP;
        }

        auto archive = make<ZipArchive>(Path::parse(path));

        if (!archive->valid())
        {
            System::errln("{}: Failed to read zip archive '{}'", argv[0], path);
            process_exit(PROCESS_FAILURE);
            return Iteration::STOP;
        }

        unsigned int i = 0;
        for (const auto &entry : archive->entries())
        {
            System::outln("{}: Entry: {} is being extracted...", argv[0], entry.name);

            auto result = archive->extract(i, entry.name.cstring());
            if (result != Result::SUCCESS)
            {
                System::errln("{}: Failed to extract entry '{}' with error '{}'", argv[0], entry.name, get_result_description(result));
                process_exit(PROCESS_FAILURE);
            }
            i++;
        }
        return Iteration::CONTINUE;
    });

    return PROCESS_SUCCESS;
}
