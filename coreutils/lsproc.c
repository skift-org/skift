/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <abi/Task.h>

#include <libsystem/cmdline.h>
#include <libsystem/Result.h>
#include <libsystem/io/Stream.h>

static bool option_cpu_usage = false;
static bool option_human = false;
static bool option_name = false;
static bool option_show_cwd = false;
static bool option_state = false;
static bool option_list = false;
static bool option_all = false;

/* --- Command line application initialization -------------------------------*/

static const char *usages[] = {
    "",
    "OPTION...",
    "OPTION... PID...",
    NULL,
};

void all_cmdline_callback(CommandLine *cmdline, CommandLineOption *opt)
{
    __unused(cmdline);
    __unused(opt);

    option_cpu_usage = true;
    option_human = true;
    option_name = true;
    option_show_cwd = true;
    option_state = true;
    option_list = true;
}

void full_cmdline_callback(CommandLine *cmdline, CommandLineOption *opt)
{
    __unused(cmdline);
    __unused(opt);

    option_cpu_usage = true;
    option_human = false;
    option_name = true;
    option_show_cwd = true;
    option_state = true;
    option_list = true;
}

static CommandLineOption options[] = {
    COMMANDLINE_OPT_HELP,

    COMMANDLINE_OPT_BOOL("cwd", 'w', option_show_cwd, "Display the cwd.", COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_BOOL("human", 'v', option_human, "Display in a human readable way.", COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_BOOL("list", 'l', option_list, "Display in a list.", COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_BOOL("name", 'n', option_name, "Display the name of the process.", COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_BOOL("state", 's', option_state, "Display the state of the process.", COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_BOOL("usage", 'u', option_cpu_usage, "Display cpu usage in %.", COMMANDLINE_NO_CALLBACK),

    COMMANDLINE_OPT_SEPARATOR,

    COMMANDLINE_OPT_BOOL("all", 'a', option_all, "Display all the above options.", all_cmdline_callback),
    COMMANDLINE_OPT_BOOL("full", 'f', option_all, "Display all the above options exept -v.", full_cmdline_callback),

    COMMANDLINE_OPT_END,
};

static CommandLine cmdline = CMDLINE(
    usages,
    options,
    "Display a snapshot of all running processes.",
    "Options can be combined.");

/* --- lsproc logic --------------------------------------------------------- */

const char *task_state_string[] = {
    "hang",
    "launchpad",
    "running",
    "sleep",
    "blocked",
    "wait",
    "wait_message",
    "wait_respond",
    "canceled",
};

int lsproc(TaskInfo *info)
{
    // process id
    if (option_human)
    {
        printf("id:");
    }
    printf("%d ", info->id);
    if (option_human)
    {
        printf("\n");
    }

    // Process state
    if (option_state)
    {
        if (option_human)
        {
            printf("\tstate:");
        }

        printf("%s ", task_state_string[info->state]);

        if (option_human)
        {
            printf("\n");
        }
    }

    // Process name
    if (option_cpu_usage)
    {
        if (option_human)
        {
            printf("\tusage: %2d%% \n", info->usage_cpu);
        }
        else
        {
            printf("%d ", info->usage_cpu);
        }
    }

    // Process name
    if (option_name)
    {
        if (option_human)
        {
            printf("\tname:");
        }

        printf("'%s' ", info->name);

        if (option_human)
        {
            printf("\n");
        }
    }

    // Process current working directory
    if (option_show_cwd)
    {
        if (option_human)
        {
            printf("\tcwd:");
        }

        printf("'%s' ", info->cwd);

        if (option_human)
        {
            printf("\n");
        }
    }

    if (option_list & !option_human)
    {
        printf("\n");
    }

    return 0;
}

/* --- Entry point ---------------------------------------------------------- */

int main(int argc, char **argv)
{
    argc = cmdline_parse(&cmdline, argc, argv);

    Stream *proc_device = stream_open("/dev/proc", OPEN_READ);

    if (handle_has_error(proc_device))
    {
        handle_printf_error(proc_device, "lproc: Failled to open /dev/proc");
        stream_close(proc_device);

        return -1;
    }

    TaskInfo info;

    while (stream_read(proc_device, &info, sizeof(info)))
    {
        lsproc(&info);
    }

    stream_close(proc_device);

    return 0;
}
