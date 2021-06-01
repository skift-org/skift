#include <abi/Syscalls.h>
#include <stdio.h>
#include <string.h>

#include <libio/Streams.h>
#include <libshell/ReadLine.h>
#include <libsystem/process/Process.h>
#include <skift/Environment.h>

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
            int command_exit_value = shell_eval(node, IO::in().handle(), IO::out().handle());

            shell_node_destroy(node);
            return command_exit_value;
        }
    }

    Shell::History history;

    int command_exit_value = 0;

    while (true)
    {
        shell_prompt(command_exit_value);
        auto line = Shell::readline(IO::in(), IO::out(), history);

        if (!line.success())
        {
            return -1;
        }

        ShellNode *node = shell_parse(line.unwrap().cstring());

        command_exit_value = shell_eval(node, IO::in().handle(), IO::out().handle());

        shell_node_destroy(node);
    }
}
