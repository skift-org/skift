#include <libio/Copy.h>
#include <libio/File.h>
#include <libio/Scanner.h>
#include <libio/Streams.h>
#include <libshell/ArgParse.h>

static bool before = false;
static String separator;

HjResult tac(IO::Reader &reader)
{
    if (separator.empty())
    {
        separator = "\n";
    }

    IO::Scanner scanner(reader);
    Vector<String> lines;

    while (!scanner.ended())
    {
        IO::MemoryWriter line_writer;
        if (before)
        {
            TRY(IO::write(line_writer, separator));
        }
        TRY(IO::copy_line(scanner, line_writer, separator, false));
        if (!before)
        {
            TRY(IO::write(line_writer, separator));
        }
        lines.push_back(line_writer.string());
    }

    for (size_t i = lines.count(); i > 0; i--)
    {
        TRY(IO::out().write(lines[i - 1].cstring(), lines[i - 1].length()));
    }

    return SUCCESS;
}

constexpr auto PROLOGUE = "Concatenate and print lines of file(s) in reverse.";
constexpr auto EPILOGUE = "If no filename provided read from input stream\nNote that command may not work as expected when \\0 is encountered";

int main(int argc, char const *argv[])
{
    Shell::ArgParse args{};
    args.should_abort_on_failure();

    args.prologue(PROLOGUE);
    args.epiloge(EPILOGUE);

    args.usage("NAME...");
    args.usage("[OPTION]... NAME...");

    args.option_string('s', "separator", "Choose the separator to be used instead of \\n", [&](String &s) {
        separator = s;
        return Shell::ArgParseResult::SHOULD_CONTINUE;
    });

    args.option_bool('b', "before", "Attach the separator before instead of after the string", [&](bool value) {
        before = value;
        return Shell::ArgParseResult::SHOULD_CONTINUE;
    });

    auto parse_result = args.eval(argc, argv);
    if (parse_result != Shell::ArgParseResult::SHOULD_CONTINUE)
    {
        return parse_result == Shell::ArgParseResult::SHOULD_FINISH ? PROCESS_SUCCESS : PROCESS_FAILURE;
    }

    HjResult result;

    if (args.argc() == 0)
    {
        result = tac(IO::in());
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

        result = tac(file);

        if (result != SUCCESS)
        {
            IO::errln("{}: {}: {}", argv[0], filepath, get_result_description(result));
            process_result = PROCESS_FAILURE;
        }
    }

    return process_result;
}
