#include <libio/Copy.h>
#include <libio/File.h>
#include <libio/Streams.h>
#include <libutils/ArgParse.h>

static bool option_count_lines = true;
static int option_count = 10;
static char option_delimiter = '\n';
static bool option_verbose = false;
static bool option_quiet = false;

Result head(String name, IO::Reader &input, bool many)
{
    if ((!many && option_verbose) && !option_quiet)
    {
        TRY(IO::outln("==> %s <==", name));
    }

    if (option_count_lines)
    {
        return IO::head(input, IO::out(), option_delimiter, option_count);
    }
    else
    {
        return IO::copy(input, IO::out(), option_count);
    }
}

int wrap_head(String name, IO::Reader &input, bool many)
{
    Result result = head(name, input, many);

    if (result != SUCCESS)
    {
        IO::errln("head: {}: {}", name, get_result_description(result));
        return PROCESS_FAILURE;
    }
    else
    {
        return PROCESS_SUCCESS;
    }
}

int main(int argc, const char *argv[])
{
    ArgParse args;

    args.should_abort_on_failure();

    args.usage("");
    args.usage("[OPTION]...");
    args.usage("[OPTION]... FILENAME...");

    args.prologue("Print the first 10 lines of each FILE to standard output. With more than one FILE, precede each with a header giving the file name.");

    args.option_int(
        'c',
        "bytes",
        "print the first NUM bytes of each file.",
        [](int value) {
            option_count = value;
            option_count_lines = false;
            return ArgParseResult::SHOULD_CONTINUE;
        });

    args.option_int(
        'n',
        "lines",
        "print the first NUM lines of each file.",
        [](int value) {
            option_count = value;
            option_count_lines = true;
            return ArgParseResult::SHOULD_CONTINUE;
        });

    args.option(option_quiet, 'q', "quiet", "Never print headers giving file names.");

    args.option(option_verbose, 'v', "verbose", "Always print headers giving file names.");

    args.option_bool(
        'z',
        "zero-terminated",
        "Line delimiter is NUL, not newline", [](bool value) {
            if (value)
            {
                option_delimiter = '\0';
            }
            else
            {
                option_delimiter = '\n';
            }
            return ArgParseResult::SHOULD_CONTINUE;
        });

    args.epiloge("If no filename provided read from standard input.\nOptions can be combined.");

    auto parse_result = args.eval(argc, argv);
    if (parse_result != ArgParseResult::SHOULD_CONTINUE)
    {
        return parse_result == ArgParseResult::SHOULD_FINISH ? PROCESS_SUCCESS : PROCESS_FAILURE;
    }

    if (args.argc() == 0)
    {
        return wrap_head("Standard Input", IO::in(), false);
    }

    for (auto filename : args.argv())
    {
        IO::File file{filename, OPEN_READ};

        if (wrap_head(filename, file, true) != PROCESS_SUCCESS)
        {
            return PROCESS_FAILURE;
        }
    }

    return PROCESS_SUCCESS;
}
