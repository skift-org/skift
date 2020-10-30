#include <libutils/ArgParse.h>
#include <libutils/Path.h>

#define PROLOGUE "Output each NAME with its last non - slash component and trailing slashes removed."
#define EPILOGE "If NAME contains no /'s, output '.' (meaning the current directory)."
#define OPTION_ZERO_DESCRIPTION "End each output line with NUL, not newline."

int main(int argc, char const *argv[])
{
    ArgParse args;

    args.prologue(PROLOGUE);

    args.usage("NAME...");
    args.usage("[OPTION] NAME...");

    auto option_zero = args.option<OptionBool>('z');
    option_zero->longname("zero");
    option_zero->description(OPTION_ZERO_DESCRIPTION);

    args.epiloge(EPILOGE);
    args.show_help_if_no_operand_given();

    args.eval(argc, argv);

    char terminator = option_zero->value() ? '\0' : '\n';

    for (size_t i = 0; i < args.argc(); i++)
    {
        auto path = Path::parse(args.argv()[i]);

        printf("%s%c", path.dirname().cstring(), terminator);
    }

    return PROCESS_SUCCESS;
}
