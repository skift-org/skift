
#include <libio/CRCReader.h>
#include <libio/Copy.h>
#include <libio/File.h>
#include <libio/Sink.h>
#include <libutils/ArgParse.h>

int main(int argc, char const *argv[])
{
    ArgParse args;

    args.show_help_if_no_operand_given();
    args.should_abort_on_failure();

    args.usage("FILES...");
    args.usage("OPTION... FILES...");

    if (args.eval(argc, argv) != PROCESS_SUCCESS)
    {
        return PROCESS_FAILURE;
    }

    for (unsigned int i = 0; i < args.argc(); i++)
    {
        IO::File file{args.argv()[i], OPEN_READ};

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