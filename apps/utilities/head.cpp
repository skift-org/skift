#include <libsystem/cmdline/CMDLine.h>
#include <libsystem/io/Stream.h>
#include <stdio.h>

#define BUFFER_SIZE 1024

static bool count_lines = true;
static int count = 10;
static bool quiet = false;
static bool verbose = false;
static bool zero_delimiter = false;
static char delimiter = '\n';

void set_delimiter_zero(CommandLine *, CommandLineOption *);
void set_character_mode(CommandLine *, CommandLineOption *);
Result head(Stream *const, char *const);

static const char *usages[] = {
    "[OPTION]... NAME...",
    nullptr,
};

static CommandLineOption options[] = {
    COMMANDLINE_OPT_HELP,
    COMMANDLINE_OPT_INT("bytes", 'c', count,
                        "Print the first NUM bytes of each file; with the leading '-', print all but the last NUM bytes of each file",
                        set_character_mode),
    COMMANDLINE_OPT_INT("lines", 'n', count,
                        "Print the first NUM bytes of each file; with the leading '-', print all but the last NUM bytes of each file",
                        COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_BOOL("quiet", 'q', quiet,
                         "Never print headers giving file names",
                         COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_BOOL("verbose", 'v', verbose,
                         "Always print headers giving file names",
                         COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_BOOL("zero-terminated", 'z', zero_delimiter,
                         "Line delimiter is NUL, not newline",
                         set_delimiter_zero),
    COMMANDLINE_OPT_END};

static CommandLine cmdline = CMDLINE(
    usages,
    options,
    "Print the first 10 lines of each FILE to standard output. With more than one FILE, precede each with\
    a header giving the file name\n",
    "If no filename provided read from input stream\n");

int main(int argc, char **argv)
{
    argc = cmdline_parse(&cmdline, argc, argv);

    if (count < 0)
    {
        /* TODO: print everthing but last COUNT lines when count is negative */
        stream_format(err_stream, "%s", get_result_description(ERR_FUNCTION_NOT_IMPLEMENTED));
        return PROCESS_FAILURE;
    }

    Result result;
    int process_status = PROCESS_SUCCESS;

    if (argc == 1)
    {
        count = 1;
        char input_stream_name[] = "Standard Input";

        stream_set_read_buffer_mode(in_stream, STREAM_BUFFERED_LINE);
        stream_set_write_buffer_mode(out_stream, STREAM_BUFFERED_NONE);
        result = head(in_stream, input_stream_name);
        if (result != SUCCESS)
        {
            stream_format(err_stream, "%s: %s: %s", argv[0], "STDIN", get_result_description(result));
            process_status = PROCESS_FAILURE;
        }

        return process_status;
    }

    if (argc > 2 && quiet == false)
    {
        verbose = true;
    }

    for (int i = 1; i < argc; i++)
    {
        __cleanup(stream_cleanup) Stream *file_stream = stream_open(argv[i], OPEN_READ);

        if (handle_has_error(file_stream))
        {
            stream_format(err_stream, "%s: %s: %s", argv[0], argv[i], get_result_description(handle_get_error(file_stream)));
            process_status = PROCESS_FAILURE;
            continue;
        }

        result = head(file_stream, argv[i]);
        if (argc > 2 && i != argc - 1)
        {
            printf("\n");
        }

        if (result != SUCCESS)
        {
            stream_format(err_stream, "%s: %s: %s", argv[0], argv[i], get_result_description(result));
            process_status = PROCESS_FAILURE;
        }
    }

    return process_status;
}

void set_delimiter_zero(CommandLine *cmdline, CommandLineOption *opt)
{
    __unused(cmdline);
    __unused(opt);

    delimiter = '\0';
}

void set_character_mode(CommandLine *cmdline, CommandLineOption *opt)
{
    __unused(cmdline);
    __unused(opt);

    count_lines = false;
}

Result head(Stream *const input_stream, char *const stream_name)
{
    char buffer[BUFFER_SIZE];

    size_t bytes_read;
    int counter = 0;

    if (verbose)
    {
        stream_format(out_stream, "==> %s <==\n", stream_name);
        if (handle_has_error(out_stream))
        {
            return handle_get_error(out_stream);
        }
    }

    // Character mode
    if (count_lines == false)
    {
        int full_reads = count / BUFFER_SIZE;
        size_t remainder = count % BUFFER_SIZE;

        while (counter < full_reads)
        {
            bytes_read = stream_read(input_stream, buffer, BUFFER_SIZE);
            if (bytes_read == 0)
            {
                break;
            }

            stream_write(out_stream, buffer, bytes_read);
            if (handle_has_error(out_stream))
            {
                return handle_get_error(out_stream);
            }

            counter++;
        }

        bytes_read = stream_read(input_stream, buffer, remainder);
        if (bytes_read)
        {
            stream_write(out_stream, buffer, bytes_read);
            if (handle_has_error(out_stream))
            {
                return handle_get_error(out_stream);
            }
        }
    }
    else // Line count mode
    {
        while (counter < count)
        {
            bytes_read = stream_read(input_stream, buffer, BUFFER_SIZE);
            if (bytes_read == 0)
            {
                break;
            }

            size_t buffer_iterator = 0;
            while (buffer_iterator < bytes_read)
            {
                if (buffer[buffer_iterator] == delimiter)
                {
                    counter++;
                    if (counter == count)
                    {
                        break;
                    }
                }
                buffer_iterator++;
            }

            stream_write(out_stream, buffer, buffer_iterator);
            if (handle_has_error(out_stream))
            {
                return handle_get_error(out_stream);
            }
        }
    }

    return SUCCESS;
}
