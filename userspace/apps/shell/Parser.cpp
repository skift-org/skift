#include <libsystem/utils/BufferBuilder.h>

#include <libutils/Scanner.h>
#include <libutils/ScannerUtils.h>

#include "shell/Nodes.h"

#define SHELL_WHITESPACE " \r\n\t"

static void whitespace(Scanner &scan)
{
    scan.eat(SHELL_WHITESPACE);
}

static char *string(Scanner &scan)
{
    BufferBuilder *builder = buffer_builder_create(16);

    scan.skip('"');

    while (scan.current() != '"' &&
           scan.do_continue())
    {
        if (scan.current() == '\\')
        {
            buffer_builder_append_str(builder, scan_json_escape_sequence(scan));
        }
        else
        {
            buffer_builder_append_chr(builder, scan.current());
            scan.foreward();
        }
    }

    scan.skip('"');

    return buffer_builder_finalize(builder);
}

static char *argument(Scanner &scan)
{
    if (scan.current() == '"')
    {
        return string(scan);
    }

    BufferBuilder *builder = buffer_builder_create(16);

    while (!scan.current_is(SHELL_WHITESPACE) &&
           scan.do_continue())
    {
        if (scan.current() == '\\')
        {
            scan.foreward();
        }

        if (scan.do_continue())
        {
            buffer_builder_append_chr(builder, scan.current());
            scan.foreward();
        }
    }

    return buffer_builder_finalize(builder);
}

static ShellNode *command(Scanner &scan)
{
    char *command_name = argument(scan);

    whitespace(scan);

    List *arguments = list_create();

    whitespace(scan);

    while (scan.do_continue() &&
           scan.current() != '|' &&
           scan.current() != '>')
    {
        list_pushback(arguments, argument(scan));
        whitespace(scan);
    }

    return shell_command_create(command_name, arguments);
}

static ShellNode *pipeline(Scanner &scan)
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

static ShellNode *redirect(Scanner &scan)
{
    ShellNode *node = pipeline(scan);

    whitespace(scan);

    if (!scan.skip('>'))
    {
        return node;
    }

    whitespace(scan);

    char *destination = argument(scan);

    return shell_redirect_create(node, destination);
}

ShellNode *shell_parse(char *command_text)
{
    StringScanner scan(command_text, strlen(command_text));

    // Skip the utf8 bom header if present.
    scan_skip_utf8bom(scan);
    whitespace(scan);
    return redirect(scan);
}
