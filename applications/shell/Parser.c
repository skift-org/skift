#include <libsystem/CString.h>
#include <libsystem/utils/BufferBuilder.h>
#include <libsystem/utils/SourceReader.h>

#include "shell/Nodes.h"

#define SHELL_WHITESPACE " \r\n\t"

static void whitespace(SourceReader *source)
{
    source_eat(source, SHELL_WHITESPACE);
}

static char *string(SourceReader *source)
{
    BufferBuilder *builder = buffer_builder_create(16);

    source_skip(source, '"');

    while (source_current(source) != '"' &&
           source_do_continue(source))
    {
        if (source_current(source) == '\\')
        {
            buffer_builder_append_str(builder, source_read_escape_sequence(source));
        }
        else
        {
            buffer_builder_append_chr(builder, source_current(source));
            source_foreward(source);
        }
    }

    source_skip(source, '"');

    return buffer_builder_finalize(builder);
}

static char *argument(SourceReader *source)
{
    if (source_current(source) == '"')
    {
        return string(source);
    }
    else
    {
        BufferBuilder *builder = buffer_builder_create(16);

        while (!source_current_is(source, SHELL_WHITESPACE) &&
               source_do_continue(source))
        {
            if (source_current(source) == '\\')
            {
                source_foreward(source);
            }

            if (source_do_continue(source))
            {
                buffer_builder_append_chr(builder, source_current(source));
                source_foreward(source);
            }
        }

        return buffer_builder_finalize(builder);
    }
}

static ShellNode *command(SourceReader *source)
{
    char *command_name = argument(source);

    whitespace(source);

    List *arguments = list_create();

    whitespace(source);

    while (source_do_continue(source))
    {
        list_pushback(arguments, argument(source));
        whitespace(source);
    }

    return shell_node_command_create(command_name, arguments);
}

static ShellNode *operator_(SourceReader *source)
{
    ShellNode *left = command(source);
    whitespace(source);

    if (source_current(source) != '|')
    {
        return left;
    }

    ShellNode *right = command(source);

    return shell_node_operator_create(SHELL_NODE_PIPE, left, right);
}

ShellNode *shell_parse(char *command_text)
{
    SourceReader *source = source_create_from_string(command_text, strlen(command_text));

    // Skip the utf8 bom header if present.
    source_skip_word(source, "\xEF\xBB\xBF");

    whitespace(source);

    ShellNode *node = operator_(source);

    source_destroy(source);

    return node;
}
