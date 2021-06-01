
#include <libio/CRCReader.h>
#include <libio/Copy.h>
#include <libio/File.h>
#include <libio/Sink.h>
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

    for (unsigned int i = 0; i < args.argc(); i++)
    {
        IO::File file{args.argv()[i], HJ_OPEN_READ};

        if (!file.exist())
        {
            IO::errln("{}: File does not exist", file.path().unwrap().string());
            return PROCESS_FAILURE;
        }

        IO::Sink sink;
        IO::CRCReader crc_reader(file);
        IO::copy(crc_reader, sink);

        IO::outln("{}:\t{}", crc_reader.checksum(), file.path().unwrap().string());
    }

    return PROCESS_SUCCESS;
}