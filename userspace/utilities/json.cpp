#include <libio/File.h>
#include <libio/Streams.h>
#include <libjson/Json.h>
#include <libshell/ArgParse.h>

constexpr auto PROLOGUE = "Reformats JSON to make it easier to read.";

constexpr auto OPTION_COLOR_DESCRIPTION = "Color json levels using VT100 sequences.";
constexpr auto OPTION_IDENT_DESCRIPTION = "Ident json levels.";

constexpr auto EPILOGUE = "Options can be combined";

int main(int argc, char const *argv[])
{
    Shell::ArgParse args{};

    args.should_abort_on_failure();
    args.show_help_if_no_option_given();

    args.prologue(PROLOGUE);

    args.usage("");
    args.usage("OPTION...");
    args.usage("OPTION... FILES...");

    auto options = IO::Prettier::NONE;

    args.option('c', "color", OPTION_COLOR_DESCRIPTION, [&](auto &) {
        options |= IO::Prettier::COLORS;
        return Shell::ArgParseResult::SHOULD_CONTINUE;
    });

    args.option('i', "indent", OPTION_IDENT_DESCRIPTION, [&](auto &) {
        options |= IO::Prettier::INDENTS;
        return Shell::ArgParseResult::SHOULD_CONTINUE;
    });

    args.epiloge(EPILOGUE);

    auto parse_result = args.eval(argc, argv);
    if (parse_result != Shell::ArgParseResult::SHOULD_CONTINUE)
    {
        return parse_result == Shell::ArgParseResult::SHOULD_FINISH ? PROCESS_SUCCESS : PROCESS_FAILURE;
    }

    if (args.argc() == 0)
    {
        auto root = Json::parse(IO::in());

        IO::MemoryWriter memory;
        IO::Prettier pretty{memory, options};
        Json::prettify(pretty, root);
        IO::write(IO::out(), memory.slice());
    }
    else
    {
        args.argv().foreach([&](auto &path) {
            IO::File file{path, HJ_OPEN_READ};

            if (!file.exist())
            {
                return Iteration::CONTINUE;
            }

            auto root = Json::parse(file);

            IO::MemoryWriter memory;
            IO::Prettier pretty{memory, options};
            Json::prettify(pretty, root);
            IO::write(IO::out(), memory.slice());

            return Iteration::CONTINUE;
        });
    }

    return PROCESS_SUCCESS;
}
