#include <string.h>
#include <libsystem/io/Stream.h>

int matchstar(int c, char *re, char *text);

// matchhere: search for re at beginning of text
int matchhere(char *re, char *text)
{
    if (re[0] == '\0')
    {
        return 1;
    }

    if (re[1] == '*')
    {
        return matchstar(re[0], re + 2, text);
    }

    if (re[0] == '$' && re[1] == '\0')
    {
        return *text == '\0';
    }

    if (*text != '\0' && (re[0] == '.' || re[0] == *text))
    {
        return matchhere(re + 1, text + 1);
    }

    return 0;
}

// matchstar: search for c*re at beginning of text
int matchstar(int c, char *re, char *text)
{
    do
    {
        // a * matches zero or more instances
        if (matchhere(re, text))
        {
            return 1;
        }
    } while (*text != '\0' && (*text++ == c || c == '.'));

    return 0;
}

int match(char *re, char *text)
{
    if (re[0] == '^')
    {
        return matchhere(re + 1, text);
    }

    do
    {
        // must look at empty string
        if (matchhere(re, text))
        {
            return 1;
        }
    } while (*text++ != '\0');

    return 0;
}

void grep(char *pattern, Stream *stream)
{
    char *q;

    char buffer[1024];
    size_t file_offset = 0;
    size_t text_read = stream_read(stream, buffer + file_offset, sizeof(buffer) - file_offset - 1);

    while (text_read > 0)
    {
        file_offset += text_read;
        buffer[file_offset] = '\0';
        char *p = buffer;

        while ((q = strchr(p, '\n')) != 0)
        {
            *q = 0;

            if (match(pattern, p))
            {
                *q = '\n';
                stream_write(out_stream, p, q + 1 - p);
            }

            p = q + 1;
        }

        if (p == buffer)
        {
            file_offset = 0;
        }

        if (file_offset > 0)
        {
            file_offset -= p - buffer;
            memmove(buffer, p, file_offset);
        }

        text_read = stream_read(stream, buffer + file_offset, sizeof(buffer) - file_offset - 1);
    }
}

int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        stream_format(err_stream, "usage: grep pattern [file ...]\n");
        return 0;
    }

    char *pattern = argv[1];

    if (argc <= 2)
    {
        grep(pattern, in_stream);
        return 0;
    }

    for (int i = 2; i < argc; i++)
    {
        Stream *stream = stream_open(argv[i], OPEN_READ);

        if (handle_has_error(stream))
        {
            handle_printf_error(stream, "grep: cannot open %s", argv[i]);
            stream_close(stream);

            return -1;
        }

        grep(pattern, stream);
        stream_close(stream);
    }

    return 0;
}
