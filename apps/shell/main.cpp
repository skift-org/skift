#include <abi/Syscalls.h>

#include <libsystem/Logger.h>
#include <libsystem/cmdline/ReadLine.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Process.h>
#include <skift/Environment.h>

#include <stdio.h>
#include <string.h>

#include "shell/Shell.h"

const char8_t *PROMPT = u8"\e[;1;94m µ \e[m";

void shell_prompt(int last_command_exit_value)
{
    printf("\n\e[m");

    if (last_command_exit_value != 0)
    {
        printf("(\e[;1;31m%d\e[m) ", last_command_exit_value);
    }

    SystemInfo info{};
    hj_system_info(&info);
    auto user_name = environment().get("POSIX").get("LOGNAME").as_string();

    printf("\e[94m%s\e[m@%s ", user_name.cstring(), info.machine);

    char buffer[PATH_LENGTH];
    process_get_directory(buffer, PATH_LENGTH);
    printf("%s", buffer);

    // FIXME: We should use char8_t
    printf((const char *)PROMPT);
}

int main(int argc, char **argv)
{
    if (argc == 3)
    {
        if (strcpy(argv[1], "-c"))
        {
            ShellNode *node = shell_parse(argv[2]);
            int command_exit_value = shell_eval(node, in_stream, out_stream);

            shell_node_destroy(node);
            return command_exit_value;
        }
    }

    stream_set_write_buffer_mode(out_stream, STREAM_BUFFERED_NONE);

    ReadLine *readline = readline_create(in_stream);

    int command_exit_value = 0;

    while (true)
    {
        shell_prompt(command_exit_value);
        char *command = nullptr;
        if (readline_readline(readline, &command) != SUCCESS)
        {
            return -1;
        }

        ShellNode *node = shell_parse(command);

        if (command)
        {
            free(command);
        }

        command_exit_value = shell_eval(node, in_stream, out_stream);

        shell_node_destroy(node);
    }
}
