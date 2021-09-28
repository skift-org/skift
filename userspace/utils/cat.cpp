
#include <abi/Result.h>
#include <libio/Copy.h>
#include <libio/File.h>
#include <libio/Streams.h>
#include <libshell/ArgParse.h>
#include <libutils/Array.h>

static bool option_linenumbers = false;

HjResult cat(IO::Reader &reader)
{
    if (option_linenumbers)
    {
        IO::Scanner scanner(reader);
        size_t line = 1;

        while (!scanner.ended())
        {
            TRY(IO::format(IO::out(), "{0} ", line));
            TRY(IO::copy_line(scanner, IO::out()));
            line++;
        }

        return HjResult::SUCCESS;
    }
    else
    {
        return IO::copy(reader, IO::out());
    }
}

int main(int argc, char const *argv[])
{
    Shell::ArgParse args;
    args.should_abort_on_failure();

    args.prologue("Concatenate FILE(s) to standard output.");

    args.usage("OPTION... FILE...");

    args.option(option_linenumbers, 'n', "number", "Number all output lines");

    auto parse_result = args.eval(argc, argv);
    if (parse_result != Shell::ArgParseResult::SHOULD_CONTINUE)
    {
        return parse_result == Shell::ArgParseResult::SHOULD_FINISH ? PROCESS_SUCCESS : PROCESS_FAILURE;
    }

    HjResult result;
    if (args.argc() == 0)
    {
        result = cat(IO::in());
        if (result != SUCCESS)
        {
            IO::errln("{}: {}: {}", argv[0], "STDIN", get_result_description(result));
            return PROCESS_FAILURE;
        }
    }

    int process_result = PROCESS_SUCCESS;
    for (auto filepath : args.argv())
    {
        IO::File file(filepath, HJ_OPEN_READ);

        if (file.result() != HjResult::SUCCESS)
        {
            IO::errln("{}: {}: {}", argv[0], filepath, get_result_description(file.result()));
            process_result = PROCESS_FAILURE;
            continue;
        }

        result = cat(file);

        if (result != SUCCESS)
        {
            IO::errln("{}: {}: {}", argv[0], filepath, get_result_description(result));
            process_result = PROCESS_FAILURE;
        }
    }

    return process_result;
}
