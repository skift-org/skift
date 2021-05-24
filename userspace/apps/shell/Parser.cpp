#include <libio/MemoryReader.h>
#include <libio/MemoryWriter.h>
#include <libio/Scanner.h>
#include <libio/Write.h>
#include <libutils/String.h>

#include "shell/Nodes.h"

#define SHELL_WHITESPACE " \r\n\t"

static void whitespace(IO::Scanner &scan)
{
    scan.eat_any(SHELL_WHITESPACE);
}

static String quote_string(IO::Scanner &scan)
{
    IO::MemoryWriter memory{16};

    scan.skip('"');

    while (scan.peek() != '"' && !scan.ended())
    {
        IO::write(memory, scan.next());
    }

    scan.skip('"');

    return memory.string();
}

static String word_string(IO::Scanner &scan)
{
    IO::MemoryWriter memory{16};

    while (!scan.peek_is_any(SHELL_WHITESPACE) &&
           !scan.ended())
    {
        scan.skip('\'');

        if (!scan.ended())
        {
            IO::write(memory, scan.next());
        }
    }

    return memory.string();
}

static String argument(IO::Scanner &scan)
{
    if (scan.peek() == '"')
    {
        return quote_string(scan);
    }
    else
    {
        return word_string(scan);
    }
}

static ShellNode *command(IO::Scanner &scan)
{
    char *command_name = strdup(argument(scan).cstring());

    whitespace(scan);

    List *arguments = list_create();

    whitespace(scan);

    while (!scan.ended() &&
           scan.peek() != '|' &&
           scan.peek() != '>')
    {
        list_pushback(arguments, strdup(argument(scan).cstring()));
        whitespace(scan);
    }

    return shell_command_create(command_name, arguments);
}

static ShellNode *pipeline(IO::Scanner &scan)
{
    List *commands = list_create();

    do
    {
        whitespace(scan);
        list_pushback(commands, command(scan));
        whitespace(scan);
    } while (scan.skip('|'));

    if (commands->count() == 1)
    {
        ShellNode *node = (ShellNode *)list_peek(commands);
        list_destroy(commands);
        return node;
    }

    return shell_pipeline_create(commands);
}

static ShellNode *redirect(IO::Scanner &scan)
{
    ShellNode *node = pipeline(scan);

    whitespace(scan);

    if (!scan.skip('>'))
    {
        return node;
    }

    whitespace(scan);

    char *destination = strdup(argument(scan).cstring());

    return shell_redirect_create(node, destination);
}

ShellNode *shell_parse(const char *command_text)
{
    IO::MemoryReader memory{command_text};
    IO::Scanner scan{memory};

    // Skip the utf8 bom header if present.
    whitespace(scan);
    return redirect(scan);
}
