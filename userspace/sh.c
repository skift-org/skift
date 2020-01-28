/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <abi/Process.h>

#include <libdevice/keyboard.h>
#include <libsystem/convert.h>
#include <libsystem/cstring.h>
#include <libsystem/error.h>
#include <libsystem/io/Stream.h>
#include <libsystem/logger.h>
#include <libsystem/messaging.h>
#include <libsystem/process/Launchpad.h>
#include <libsystem/process/Process.h>
#include <libsystem/readline/ReadLine.h>

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

    ReadLine *readline;
    int command_argc;
    char **command_argv;

    int command_exit_value;
} shell_t;

/* --- Prompt --------------------------------------------------------------- */

#define PROMPT u8"\e[0;94m µ \e[m"

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

/* --- Tokenizer ------------------------------------------------------------ */

int shell_split(shell_t *this, char *command)
{
    strleadtrim(command, ' ');
    strtrailtrim(command, ' ');

    if (strlen(command) == 0)
        return 0;

    int token_index = 0;

    char **tokens = malloc(PROCESS_ARG_COUNT * sizeof(char *));
    char *start = &command[0];

    for (size_t i = 0; i < strlen(command) + 1; i++)
    {
        char c = command[i];

        if (c == ' ' || c == '\0')
        {
            int buffer_len = &command[i] - start + 1;

            if (buffer_len > 1)
            {
                char *buffer = malloc(buffer_len);
                memcpy(buffer, start, buffer_len);
                buffer[buffer_len - 1] = '\0';

                tokens[token_index++] = buffer;
            }

            start = &command[i] + 1;
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
            stream_printf(err_stream, "cd: Cannot access '%s'", argv[1]);
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
            stream_printf(err_stream, "cd: Cannot access '%s'", "/");
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

void shell_eval(shell_t *this)
{
    shell_builtin_handler_t builtin = shell_get_builtin(this->command_argv[0]);

    if (builtin != NULL)
    {
        this->command_exit_value = builtin(this, this->command_argc, (const char **)this->command_argv);

        return;
    }

    char executable[PATH_LENGHT];
    snprintf(executable, PATH_LENGHT, "/bin/%s", this->command_argv[0]);

    Launchpad *launchpad = launchpad_create(this->command_argv[0], executable);

    for (int i = 0; i < this->command_argc; i++)
    {
        launchpad_argument(launchpad, this->command_argv[i]);
    }

    int process = launchpad_launch(launchpad);

    if (process < 0)
    {
        printf("%s: Command not found! \e[90m%s\e[m\n", this->command_argv[0], error_to_string(-process));
        this->command_exit_value = -1;
    }
    else
    {
        process_wait(process, &this->command_exit_value);
    }
}

/* --- Entry point ---------------------------------------------------------- */

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    logger_level(LOGGER_TRACE);

    stream_set_write_buffer_mode(out_stream, STREAM_BUFFERED_NONE);

    shell_t this = {
        .do_continue = true,
        .builtins = shell_get_builtins(),
        .command_exit_value = 0,
        .readline = readline_create(),
    };

    while (this.do_continue)
    {
        shell_prompt(&this);

        char *command = readline_readline(this.readline);
        strleadtrim(command, ' ');
        strtrailtrim(command, ' ');

        if (strlen(command) == 0)
        {
            printf("Empty command!\n");

            this.command_exit_value = -1;
            continue;
        }

        if (shell_split(&this, command) == 0)
        {
            printf("Empty command!\n");

            this.command_exit_value = -1;
            continue;
        }

        shell_eval(&this);

        shell_cleanup(&this);
        free(command);
    }

    stream_printf(err_stream, "\n[ sh exited %d ]\n", this.command_exit_value);
    return this.command_exit_value;
}