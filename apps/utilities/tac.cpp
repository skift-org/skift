#include <libsystem/cmdline/CMDLine.h>
#include <libsystem/io/Stream.h>
#include <libutils/Path.h>
#include <libutils/String.h>
#include <libutils/StringBuilder.h>
#include <libutils/Vector.h>

#include <cstring>

static bool before = false;
static char *separator = nullptr;

static const char *usages[] = {
    "NAME...",
    "[OPTION]... NAME...",
    nullptr,
};

static CommandLineOption options[] = {
    COMMANDLINE_OPT_HELP,
    COMMANDLINE_OPT_STRING("separator", 's', separator,
                           "Choose the separator[STRING] to be used instead of \\n",
                           COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_BOOL("before", 'b', before,
                         "Attach the separator before instead of after the string",
                         COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_END};

static CommandLine cmdline = CMDLINE(
    usages,
    options,
    "Concatenate and print lines of file(s) in reverse.",
    "If no filename provided read from input stream\nNote that command may not work as expected when \\0 is encountered");

char *str_split(char *str, char *const sub_str)
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

Result tac(Stream *const input_stream)
{
    size_t read;
    char buffer[1024];
    char *split = nullptr;

    StringBuilder temp;
    Vector<String> lines(20);
    if (!separator)
    {
        separator = new char[2];
        strcpy(separator, "\n");
    }

    while ((read = stream_read(input_stream, buffer, 1024 - 1)) != 0)
    {

        buffer[read] = 0;
        split = str_split(buffer, separator);
        while (split != nullptr)
        {
            temp.append(split);
            split = str_split(NULL, separator);
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
        stream_write(out_stream, lines[i - 1].cstring(), lines[i - 1].length());
        if (handle_has_error(out_stream))
        {
            return handle_get_error(out_stream);
        }
    }

    return SUCCESS;
}

int main(int argc, char **argv)
{
    argc = cmdline_parse(&cmdline, argc, argv);
    Result result;
    int process_status = PROCESS_SUCCESS;

    if (argc == 1)
    {
        result = tac(in_stream);
        if (result != SUCCESS)
        {
            stream_format(err_stream, "%s: %s: %s", argv[0], "STDIN", get_result_description(result));
            process_status = PROCESS_FAILURE;
        }

        return process_status;
    }

    for (int i = 1; i < argc; i++)
    {
        __cleanup(stream_cleanup) Stream *stream = stream_open(argv[i], OPEN_READ);

        if (handle_has_error(stream))
        {
            stream_format(err_stream, "%s: %s: %s", argv[0], argv[i], get_result_description(handle_get_error(stream)));
            process_status = PROCESS_FAILURE;
            continue;
        }

        result = tac(stream);

        if (result != SUCCESS)
        {
            stream_format(err_stream, "%s: %s: %s", argv[0], argv[i], get_result_description(result));
            process_status = PROCESS_FAILURE;
        }
    }

    return process_status;
}
