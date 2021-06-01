
#include <libfile/ZipArchive.h>
#include <libio/File.h>
#include <libio/Path.h>
#include <libio/Streams.h>
#include <libshell/ArgParse.h>

int main(int argc, char const *argv[])
{
    Shell::ArgParse args;

    args.show_help_if_no_operand_given();
    args.should_abort_on_failure();

    args.usage("FILES...");
    args.usage("OPTION... FILES...");

    auto parse_result = args.eval(argc, argv);
    if (parse_result != Shell::ArgParseResult::SHOULD_CONTINUE)
    {
        return parse_result == Shell::ArgParseResult::SHOULD_FINISH ? PROCESS_SUCCESS : PROCESS_FAILURE;
    }

    // Unzip all archives that were passed as arguments
    args.argv().foreach([&](auto &path) {
        IO::errln("{}: Unzip '{}'", argv[0], path);

        IO::File file{path};
        if (!file.exist())
        {
            IO::errln("{}: File does not exist '{}'", argv[0], path);
            process_exit(PROCESS_FAILURE);
            return Iteration::STOP;
        }

        auto archive = make<ZipArchive>(IO::Path::parse(path));

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
            IO::File dest_file(entry.name, HJ_OPEN_WRITE | HJ_OPEN_CREATE);

            auto result = archive->extract(i, dest_file);
            if (result != HjResult::SUCCESS)
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
