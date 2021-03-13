#include <libio/Streams.h>
#include <libutils/ArgParse.h>
#include <libutils/json/Json.h>

constexpr auto PROLOGUE = "Reformats JSON to make it easier to read.";

constexpr auto OPTION_COLOR_DESCRIPTION = "Color json levels using VT100 sequences.";
constexpr auto OPTION_IDENT_DESCRIPTION = "Ident json levels.";

constexpr auto EPILOGUE = "Options can be combined";

int main(int argc, char const *argv[])
{
    ArgParse args{};
    args.should_abort_on_failure();

    args.prologue(PROLOGUE);

    args.usage("");
    args.usage("OPTION...");
    args.usage("OPTION... FILES...");

    auto options = Prettifier::NONE;

    args.option('c', "color", OPTION_COLOR_DESCRIPTION, [&](auto &) {
        options |= Prettifier::COLORS;
        return PROCESS_SUCCESS;
    });

    args.option('i', "indent", OPTION_IDENT_DESCRIPTION, [&](auto &) {
        options |= Prettifier::INDENTS;
        return PROCESS_SUCCESS;
    });

    args.epiloge(EPILOGUE);

    args.eval(argc, argv);

    if (args.argc() == 0)
    {
        StreamScanner scanner{in_stream};
        auto root = json::parse(scanner);

        Prettifier pretty{options};
        json::prettify(pretty, root);
        IO::write(IO::out(), pretty.finalize());
    }
    else
    {
        args.argv().foreach ([&](auto &path) {
            auto root = json::parse_file(path);

            Prettifier pretty{options};
            json::prettify(pretty, root);
            IO::write(IO::out(), pretty.finalize());

            return Iteration::CONTINUE;
        });
    }

    return PROCESS_SUCCESS;
}
