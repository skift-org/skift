#include <libsystem/cmdline/CMDLine.h>
#include <libsystem/io/Path.h>
#include <libsystem/io/Stream.h>
#include <libutils/String.h>
#include <libutils/Vector.h>

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
                           "choose the separator to be used instead of \\n",
                           COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_BOOL("before", 'b', before,
                         "attach the separator before instead of after the string",
                         COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_END};

static CommandLine cmdline = CMDLINE(
    usages,
    options,
    "Concatenate and print lines of file(s) in reverse.",
    "If no filename provided read from input stream");

Result tac(const char *path)
{

    __cleanup(stream_cleanup) Stream *stream = stream_open(path, OPEN_READ);

    if (handle_has_error(stream))
    {
        return handle_get_error(stream);
    }
    FileState stat = {};
    stream_stat(stream, &stat);

    size_t read;
    char buffer[1024];
    char sep = (separator == nullptr ? '\n' : separator[0]);

    String s;
    Vector<String> lines(20);

    while ((read = stream_read(stream, &buffer, 1024)) != 0)
    {
        if (handle_has_error(stream))
        {
            return handle_get_error(stream);
        }
        String tmp(buffer, read);
        s += tmp;
    }

    String *tmp = new String();
    for (unsigned int i = 0; i < s.length(); ++i)
    {
        String tmp2(s[i]);
        if (s[i] == sep || i == s.length() - 1)
        {
            if (!before)
            {
                *tmp += tmp2;
            }
            lines.push_back((*tmp));
            delete tmp;
            tmp = new String();
            if (before)
            {
                *tmp += tmp2;
            }
        }
        else
        {
            *tmp += tmp2;
        }
    }

    for (unsigned int i = lines.count(); i >= 1; --i)
    {
        stream_write(out_stream, lines[i - 1].cstring(), lines[i - 1].length());
        if (handle_has_error(out_stream))
        {
            return ERR_WRITE_STDOUT;
        }
    }

    stream_flush(out_stream);

    return SUCCESS;
}

int main(int argc, char **argv)
{
    argc = cmdline_parse(&cmdline, argc, argv);

    if (argc == 1)
    {
        /* TODO: Handle input from STDIN */
        return PROCESS_SUCCESS;
    }

    Result result;

    for (int i = 1; i < argc; i++)
    {
        result = tac(argv[i]);

        if (result != SUCCESS)
        {
            stream_format(err_stream, "%s: %s: %s", argv[0], argv[i], get_result_description(result));
            return PROCESS_FAILURE;
        }
    }

    return PROCESS_SUCCESS;
}
