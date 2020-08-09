#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/utils/BufferBuilder.h>
#include <libsystem/utils/Lexer.h>

#include "shell/Nodes.h"

#define SHELL_WHITESPACE " \r\n\t"

static void whitespace(Lexer &lexer)
{
    lexer.eat(SHELL_WHITESPACE);
}

static char *string(Lexer &lexer)
{
    BufferBuilder *builder = buffer_builder_create(16);

    lexer.skip('"');

    while (lexer.current() != '"' &&
           lexer.do_continue())
    {
        if (lexer.current() == '\\')
        {
            buffer_builder_append_str(builder, lexer.read_escape_sequence());
        }
        else
        {
            buffer_builder_append_chr(builder, lexer.current());
            lexer.foreward();
        }
    }

    lexer.skip('"');

    return buffer_builder_finalize(builder);
}

static char *argument(Lexer &lexer)
{
    if (lexer.current() == '"')
    {
        return string(lexer);
    }

    BufferBuilder *builder = buffer_builder_create(16);

    while (!lexer.current_is(SHELL_WHITESPACE) &&
           lexer.do_continue())
    {
        if (lexer.current() == '\\')
        {
            lexer.foreward();
        }

        if (lexer.do_continue())
        {
            buffer_builder_append_chr(builder, lexer.current());
            lexer.foreward();
        }
    }

    return buffer_builder_finalize(builder);
}

static ShellNode *command(Lexer &lexer)
{
    char *command_name = argument(lexer);

    whitespace(lexer);

    List *arguments = list_create();

    whitespace(lexer);

    while (lexer.do_continue() &&
           lexer.current() != '|' &&
           lexer.current() != '>')
    {
        list_pushback(arguments, argument(lexer));
        whitespace(lexer);
    }

    return shell_command_create(command_name, arguments);
}

static ShellNode *pipeline(Lexer &lexer)
{
    List *commands = list_create();

    do
    {
        whitespace(lexer);
        list_pushback(commands, command(lexer));
        whitespace(lexer);
    } while (lexer.skip('|'));

    if (commands->count() == 1)
    {
        ShellNode *node = (ShellNode *)list_peek(commands);
        list_destroy(commands);
        return node;
    }

    return shell_pipeline_create(commands);
}

static ShellNode *redirect(Lexer &lexer)
{
    ShellNode *node = pipeline(lexer);

    whitespace(lexer);

    if (!lexer.skip('>'))
    {
        return node;
    }

    whitespace(lexer);

    char *destination = argument(lexer);

    return shell_redirect_create(node, destination);
}

ShellNode *shell_parse(char *command_text)
{

    Lexer lexer(command_text, strlen(command_text));

    // Skip the utf8 bom header if present.
    lexer.skip_word("\xEF\xBB\xBF");
    whitespace(lexer);
    return redirect(lexer);
}
