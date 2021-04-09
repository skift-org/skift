#include <libio/File.h>
#include <libio/Streams.h>
#include <libutils/ArgParse.h>
#include <libutils/Path.h>
#include <libutils/String.h>
#include <libutils/StringBuilder.h>
#include <libutils/Vector.h>

#include <cstring>

static bool before = false;
static String separator;

char *str_split(char *str, const char *sub_str)
{
    /*
    Function to split string into parts with sub_str as delimiter
    Similar to strtok in string.h
    May not work as expected when the str contains '\0' in between
    */
    static char *start = nullptr;

    if (start == nullptr)
    {
        start = str;
    }

    if (!*start)
    {
        return nullptr;
    }

    char *split = strstr(start, sub_str);

    if (split)
    {
        if (*(split + strlen(sub_str)))
        {
            (*split) = 0;
        }
        char *tmp = start;
        start = split + strlen(sub_str);
        return tmp;
    }

    int len = strlen(start);
    if (len)
    {
        char *tmp = start;
        start += len;
        return tmp;
    }

    return nullptr;
}

Result tac(IO::Reader &reader)
{
    size_t read;
    char buffer[1024];
    char *split = nullptr;

    StringBuilder temp;
    Vector<String> lines(20);
    if (separator.empty())
    {
        separator = "\n";
    }

    while ((read = TRY(reader.read(buffer, 1024 - 1))) != 0)
    {
        buffer[read] = 0;
        split = str_split(buffer, separator.cstring());
        while (split != nullptr)
        {
            temp.append(split);
            split = str_split(NULL, separator.cstring());
            if (!before && split)
            {
                temp.append(separator);
            }

            lines.push_back(temp.finalize());
            if (before && split)
            {
                temp.append(separator);
            }
        }
        if (temp.finalize().length())
        {
            lines.push_back(temp.finalize());
        }
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
    ArgParse args{};
    args.should_abort_on_failure();
    args.show_help_if_no_operand_given();

    args.prologue(PROLOGUE);
    args.epiloge(EPILOGUE);

    args.usage("NAME...");
    args.usage("[OPTION]... NAME...");

    args.option_string('s', "separator", "Choose the separator to be used instead of \\n", [&](String &s) {
        separator = s;
        return PROCESS_SUCCESS;
    });

    args.option_bool('b', "before", "Attach the separator before instead of after the string", [&](bool value) {
        before = value;
        return PROCESS_SUCCESS;
    });

    args.eval(argc, argv);
    Result result;
    int process_status = PROCESS_SUCCESS;

    if (args.argc() == 0)
    {
        result = tac(IO::in());
        if (result != SUCCESS)
        {
            IO::errln("{}: {}: {}", argv[0], "STDIN", get_result_description(result));
            process_status = PROCESS_FAILURE;
        }

        return process_status;
    }

    for (int i = 1; i < argc; i++)
    {
        IO::File file(argv[i], OPEN_READ);

        if (file.result() != Result::SUCCESS)
        {
            IO::errln("{}: {}: {}", argv[0], argv[i], get_result_description(file.result()));
            process_status = PROCESS_FAILURE;
            continue;
        }

        result = tac(file);

        if (result != SUCCESS)
        {
            IO::errln("{}: {}: {}", argv[0], argv[i], get_result_description(result));
            process_status = PROCESS_FAILURE;
        }
    }

    return process_status;
}
