
#include <libfile/ZipArchive.h>
#include <libsystem/io/File.h>
#include <libsystem/io/Filesystem.h>
#include <libsystem/io/Stream.h>
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
        stream_format(err_stream, "%s: Unzip '%s'\n", argv[0], path.cstring());

        File file{path};
        if (!file.exist())
        {
            stream_format(err_stream, "%s: File does not exist '%s'", argv[0], path.cstring());
            process_exit(PROCESS_FAILURE);
            return Iteration::STOP;
        }

        auto archive = make<ZipArchive>(Path::parse(path));

        if (!archive->valid())
        {
            stream_format(err_stream, "%s: Failed to read zip archive '%s'", argv[0], path.cstring());
            process_exit(PROCESS_FAILURE);
            return Iteration::STOP;
        }

        unsigned int i = 0;
        for (const auto &entry : archive->entries())
        {
            stream_format(err_stream, "%s: Entry: %s is being extracted...\n", argv[0], entry.name.cstring());

            auto result = archive->extract(i, entry.name.cstring());
            if (result != Result::SUCCESS)
            {
                stream_format(err_stream, "%s: Failed to extract entry '%s' with error '%s'", argv[0], entry.name.cstring(), get_result_description(result));
                process_exit(PROCESS_FAILURE);
            }
            i++;
        }
        return Iteration::CONTINUE;
    });

    return PROCESS_SUCCESS;
}
