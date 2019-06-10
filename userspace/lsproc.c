#include <skift/iostream.h>
#include <skift/cmdline.h>
#include <hjert/task.h>

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

void all_cmdline_callback(cmdline_t *cmdline, cmdline_option_t *opt)
{
    UNUSED(cmdline); UNUSED(opt);

    option_cpu_usage = true;
    option_human = true;
    option_name = true;
    option_show_cwd = true;
    option_state = true;
    option_list = true;
}

void full_cmdline_callback(cmdline_t *cmdline, cmdline_option_t *opt)
{
    UNUSED(cmdline); UNUSED(opt);

    option_cpu_usage = true;
    option_human = false;
    option_name = true;
    option_show_cwd = true;
    option_state = true;
    option_list = true;
}

static cmdline_option_t options[] = {
    CMDLINE_OPT_HELP,

    CMDLINE_OPT_BOOL("cwd", 'w', option_show_cwd, "display the cwd", CMDLINE_NO_CALLBACK),
    CMDLINE_OPT_BOOL("human", 'v', option_human, "display in a human readable way", CMDLINE_NO_CALLBACK),
    CMDLINE_OPT_BOOL("list", 'l', option_list, "display in a list", CMDLINE_NO_CALLBACK),
    CMDLINE_OPT_BOOL("name", 'n', option_name, "display the name of the process", CMDLINE_NO_CALLBACK),
    CMDLINE_OPT_BOOL("state", 's', option_state, "display the state of the process", CMDLINE_NO_CALLBACK),
    CMDLINE_OPT_BOOL("usage", 'u', option_cpu_usage, "display cpu usage in %", CMDLINE_NO_CALLBACK),

    CMDLINE_OPT_BOOL("all", 'a', option_all, "display all the above options", all_cmdline_callback),
    CMDLINE_OPT_BOOL("full", 'f', option_all, "display all the above options exept v", full_cmdline_callback),

    CMDLINE_OPT_END,
};

static cmdline_t cmdline = CMDLINE(
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
    "wait",
    "wait_message",
    "canceled",
};

int lsproc(task_info_t *info)
{
    // process id
    if (option_human)
    {
        printf("id:");
    }
    printf("%d ", info->id);
    if (option_human){ printf("\n"); }

    // Process state
    if (option_state)
    {
        if (option_human)
        {
            printf("\tstate:");
        }
        
        printf("%s ", task_state_string[info->state]);

        if (option_human){ printf("\n"); }
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

        if (option_human){ printf("\n"); }
    }

    // Process current working directory
    if (option_show_cwd)
    {
        if (option_human)
        {
            printf("\tcwd:");
        }

        printf("'%s' ", info->cwd);

        if (option_human){ printf("\n"); }
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

    iostream_t *proc_device = iostream_open("/dev/proc", IOSTREAM_READ);

    task_info_t info;

    while (iostream_read(proc_device, &info, sizeof(info)))
    {
        lsproc(&info);
    }

    iostream_close(proc_device);

    return 0;
}
