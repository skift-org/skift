/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/cstring.h>
#include <libsystem/iostream.h>
#include <libsystem/logger.h>
#include <libsystem/messaging.h>
#include <libsystem/error.h>
#include <libsystem/process.h>
#include <libsystem/convert.h>

#include <libkernel/task.h>
#include <libdevice/keyboard.h>

#define MAX_COMMAND_LENGHT 128

struct shell;

typedef int (*shell_builtin_handler_t)(struct shell *shell, int argc, const char **argv);

typedef struct
{
    const char *name;
    shell_builtin_handler_t handler;
} shell_builtin_command_t;

typedef struct shell
{
    bool do_continue;
    shell_builtin_command_t *builtins;

    char *command_string;
    int command_argc;
    char **command_argv;

    int command_exit_value;
} shell_t;

/* --- Prompt --------------------------------------------------------------- */

#define PROMPT "\e[0;1;34m $ \e[0;1m"

void shell_prompt(shell_t *this)
{
    // Reset the terminal
    printf("\n\e[0m ");

    // Last command exit value
    if (this->command_exit_value != 0)
    {
        printf("\e[0;1;31m%d\e[0m ", this->command_exit_value);
    }

    // Current directory
    char buffer[1024];
    process_get_cwd(buffer, 1024);
    printf("%s", buffer);

    // The prompt
    printf(PROMPT);
}

/* --- Readline ------------------------------------------------------------- */

int shell_readline(shell_t *this)
{
    // FIXME: All user input should come from in_stream
    messaging_subscribe(KEYBOARD_CHANNEL);

    int i = 0;
    this->command_string[i] = '\0';

    while (true)
    {
        message_t msg;
        messaging_receive(&msg, 1);

        if (message_is(msg, KEYBOARD_KEYTYPED))
        {
            keyboard_event_t *event = message_payload_as(msg, keyboard_event_t);

            if (event->codepoint == '\n')
            {
                printf("\n");
                break;
            }
            else if (event->codepoint == '\b')
            {
                if (strlen(this->command_string) > 0)
                {
                    strbs(this->command_string);
                    printf("\b\e[K");
                }
            }
            else if (!(event->codepoint == '\0' || event->codepoint == '\t'))
            {
                if (strlen(this->command_string) < MAX_COMMAND_LENGHT - 1)
                {
                    strnapd(this->command_string, event->codepoint, MAX_COMMAND_LENGHT);
                    printf("%c", event->codepoint);
                }
            }
        }
    }

    messaging_unsubscribe(KEYBOARD_CHANNEL);

    printf("\e[0m");

    return strlen(this->command_string);
}

/* --- Tokenizer ------------------------------------------------------------ */

int shell_split(shell_t *this)
{
    strleadtrim(this->command_string, ' ');
    strtrailtrim(this->command_string, ' ');

    if (strlen(this->command_string) == 0)
        return 0;

    int token_index = 0;

    char **tokens = malloc(TASK_ARGV_COUNT * sizeof(char *));
    char *start = &this->command_string[0];

    for (size_t i = 0; i < strlen(this->command_string) + 1; i++)
    {
        char c = this->command_string[i];

        if (c == ' ' || c == '\0')
        {
            int buffer_len = &this->command_string[i] - start + 1;

            if (buffer_len > 1)
            {
                char *buffer = malloc(buffer_len);
                memcpy(buffer, start, buffer_len);
                buffer[buffer_len - 1] = '\0';

                tokens[token_index++] = buffer;
            }

            start = &this->command_string[i] + 1;
        }
    }

    tokens[token_index] = NULL;

    this->command_argv = tokens;
    this->command_argc = token_index;

    return token_index;
}

void shell_cleanup(shell_t *this)
{
    for (int i = 0; i < this->command_argc; i++)
    {
        free(this->command_argv[i]);
    }

    free(this->command_argv);
}

/* --- Builtin -------------------------------------------------------------- */

int shell_builtin_cd(shell_t *shell, int argc, const char **argv)
{
    __unused(shell);

    if (argc == 2)
    {
        int result = process_set_cwd(argv[1]);

        if (result < 0)
        {
            iostream_printf(err_stream, "cd: cannot access '%s'", argv[1]);
            error_print("");
            return -1;
        }

        return 0;
    }
    else
    {
        int result = process_set_cwd("/");

        if (result < 0)
        {
            iostream_printf(err_stream, "cd: cannot access '%s'", argv[1]);
            error_print("");
            return -1;
        }

        return 0;
    }
}

int shell_builtin_exit(shell_t *shell, int argc, const char **argv)
{
    shell->do_continue = false;

    if (argc == 2)
    {
        return convert_string_to_uint(argv[1], strnlen(argv[1], 32), 10);
    }
    else
    {
        return 0;
    }
}

shell_builtin_command_t shell_builtins[] = {
    {"cd", shell_builtin_cd},
    {"exit", shell_builtin_exit},
    {NULL, NULL},
};

shell_builtin_command_t *shell_get_builtins(void)
{
    return shell_builtins;
}

shell_builtin_handler_t shell_get_builtin(const char *name)
{
    for (int i = 0; shell_builtins[i].name != NULL; i++)
    {
        if (strcmp(shell_builtins[i].name, name) == 0)
        {
            return shell_builtins[i].handler;
        }
    }

    return NULL;
}

/* --- Eval ----------------------------------------------------------------- */

bool shell_eval(shell_t *this)
{
    // Try to exect a builtin
    shell_builtin_handler_t builtin = shell_get_builtin(this->command_argv[0]);

    if (builtin != NULL)
    {
        this->command_exit_value = builtin(this, this->command_argc, (const char **)this->command_argv);
        return true;
    }

    // Or exec a real command
    IOStream *s = iostream_open(this->command_argv[0], IOSTREAM_READ);

    int process = 0;

    if (s != NULL)
    {
        iostream_close(s);
        process = process_exec(this->command_argv[0], (const char **)this->command_argv);
    }
    else
    {
        char pathbuffer[144];
        snprintf(pathbuffer, 144, "/bin/%s", this->command_argv[0]);

        s = iostream_open(pathbuffer, IOSTREAM_READ);

        if (s != NULL)
        {
            iostream_close(s);
            process = process_exec(pathbuffer, (const char **)this->command_argv);
        }
    }

    if (process > 0)
    {
        process_wait(process, &this->command_exit_value);
        return true;
    }

    return false;
}

/* --- Entry point ---------------------------------------------------------- */

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    logger_level(LOGGER_TRACE);

    iostream_set_write_buffer_mode(out_stream, IOSTREAM_BUFFERED_NONE);

    shell_t this = {
        .do_continue = true,
        .builtins = shell_get_builtins(),
        .command_exit_value = 0,
        .command_string = malloc(MAX_COMMAND_LENGHT),
    };

    while (this.do_continue)
    {
        shell_prompt(&this);

        if (shell_readline(&this) == 0)
        {
            printf("Empty command!\n");

            this.command_exit_value = -1;
            continue;
        }

        if (shell_split(&this) == 0)
        {
            printf("Empty command!\n");

            this.command_exit_value = -1;
            continue;
        }

        if (!shell_eval(&this))
        {
            printf("Command not found!\n");
        }

        shell_cleanup(&this);
    }

    return this.command_exit_value;
}