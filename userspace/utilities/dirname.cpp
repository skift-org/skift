#include <libio/Path.h>
#include <libutils/ArgParse.h>

constexpr auto PROLOGUE = "Output each NAME with its last non - slash Element and trailing slashes removed.";
constexpr auto OPTION_ZERO_DESCRIPTION = "End each output line with NUL, not newline.";
constexpr auto EPILOGE = "If NAME contains no /'s, output '.' (meaning the current directory).";

int main(int argc, char const *argv[])
{
    ArgParse args;
    args.show_help_if_no_operand_given();
    args.should_abort_on_failure();

    args.prologue(PROLOGUE);

    args.usage("NAME...");
    args.usage("[OPTION] NAME...");

    bool option_zero = false;
    args.option(option_zero, 'z', "zero", OPTION_ZERO_DESCRIPTION);

    args.epiloge(EPILOGE);

    auto parse_result = args.eval(argc, argv);
    if (parse_result != ArgParseResult::SHOULD_CONTINUE)
    {
        return parse_result == ArgParseResult::SHOULD_FINISH ? PROCESS_SUCCESS : PROCESS_FAILURE;
    }

    char terminator = option_zero ? '\0' : '\n';

    for (size_t i = 0; i < args.argc(); i++)
    {
        auto path = IO::Path::parse(args.argv()[i]);

        IO::out("{}{c}", path.dirname(), terminator);
    }

    return PROCESS_SUCCESS;
}
