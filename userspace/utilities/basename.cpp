#include <libio/Path.h>
#include <libio/Streams.h>
#include <libutils/ArgParse.h>

int main(int argc, char const *argv[])
{
    ArgParse args;

    args.usage("NAME...");
    args.usage("OPTIONS... NAME...");

    args.show_help_if_no_operand_given();

    args.prologue("Print NAME with any leading directory Elements removed.");

    bool option_all = false;
    bool option_zero = false;
    args.option(option_all, 'a', "multiple", "Support multiple arguments and treat each as a NAME");
    args.option(option_zero, 'z', "zero", "End each output line with NUL, not newline");

    auto parse_result = args.eval(argc, argv);
    if (parse_result != ArgParseResult::SHOULD_CONTINUE)
    {
        return parse_result == ArgParseResult::SHOULD_FINISH ? PROCESS_SUCCESS : PROCESS_FAILURE;
    }

    for (auto filepath : args.argv())
    {
        auto basename = IO::Path::parse(filepath).basename();
        if (option_zero)
        {
            IO::write(IO::out(), basename);
        }
        else
        {
            IO::outln("{}", basename);
        }

        if (!option_all)
        {
            break;
        }
    }

    return PROCESS_SUCCESS;
}
