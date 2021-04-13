
#include <libfile/ZipArchive.h>
#include <libio/File.h>
#include <libio/Streams.h>
#include <libsystem/io/Filesystem.h>
#include <libutils/ArgParse.h>
#include <libutils/Path.h>

int main(int argc, char const *argv[])
{
    ArgParse args;

    args.show_help_if_no_operand_given();
    args.should_abort_on_failure();

    args.usage("FILES...");
    args.usage("OPTION... FILES...");

    auto parse_result = args.eval(argc, argv);
    if (parse_result != ArgParseResult::SHOULD_CONTINUE)
    {
        return parse_result == ArgParseResult::SHOULD_FINISH ? PROCESS_SUCCESS : PROCESS_FAILURE;
    }

    // Unzip all archives that were passed as arguments
    args.argv().foreach ([&](auto &path) {
        IO::errln("{}: Unzip '{}'", argv[0], path);

        IO::File file{path};
        if (!file.exist())
        {
            IO::errln("{}: File does not exist '{}'", argv[0], path);
            process_exit(PROCESS_FAILURE);
            return Iteration::STOP;
        }

        auto archive = make<ZipArchive>(Path::parse(path));

        if (!archive->valid())
        {
            IO::errln("{}: Failed to read zip archive '{}'", argv[0], path);
            process_exit(PROCESS_FAILURE);
            return Iteration::STOP;
        }

        unsigned int i = 0;
        for (const auto &entry : archive->entries())
        {
            IO::outln("{}: Entry: {} is being extracted...", argv[0], entry.name);
            IO::File dest_file(entry.name, OPEN_WRITE | OPEN_CREATE);

            auto result = archive->extract(i, dest_file);
            if (result != Result::SUCCESS)
            {
                IO::errln("{}: Failed to extract entry '{}' with error '{}'", argv[0], entry.name, get_result_description(result));
                process_exit(PROCESS_FAILURE);
            }

            i++;
        }
        return Iteration::CONTINUE;
    });

    return PROCESS_SUCCESS;
}
