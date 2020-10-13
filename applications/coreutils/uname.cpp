#include <libsystem/BuildInfo.h>
#include <libsystem/cmdline/CMDLine.h>
#include <libsystem/io/Stream.h>
#include <libsystem/system/System.h>

static bool option_all = false;
static bool option_kernel_name = false;
static bool option_kernel_release = false;
static bool option_kernel_version = false;
static bool option_machine = false;
static bool option_nodename = false;
static bool option_operating_system = false;

static const char *usages[] = {
    "",
    "OPTION...",
    nullptr,
};

static CommandLineOption options[] = {
    COMMANDLINE_OPT_HELP,

    COMMANDLINE_OPT_BOOL("all", 'a', option_all, "print all information.", COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_BOOL("kernel-name", 's', option_kernel_name, "print the kernel name.", COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_BOOL("nodename", 'n', option_nodename, "print the network node hostname", COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_BOOL("kernel-release", 'r', option_kernel_release, "print the kernel release.", COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_BOOL("kernel-version", 'v', option_kernel_version, "print the kernel version.", COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_BOOL("machine", 'm', option_machine, "print the machine hardware name.", COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_BOOL("operating-system", 'o', option_operating_system, "print the operating system.", COMMANDLINE_NO_CALLBACK),

    COMMANDLINE_OPT_END};

static CommandLine cmdline = CMDLINE(
    usages,
    options,
    "Print certain system information.  With no OPTION, same as -s.",
    "Options can be combined");

int main(int argc, char **argv)
{
    cmdline_parse(&cmdline, argc, argv);

    SystemInfo info = system_get_info();

    if (argc == 1 || option_kernel_name || option_all)
    {
        printf("%s ", info.kernel_name);
    }

    if (option_nodename || option_all)
    {
        printf("%s ", info.machine);
    }

    if (option_kernel_release || option_all)
    {
        printf("%s ", info.kernel_release);
    }

    if (option_kernel_version || option_all)
    {
        printf("%s ", info.kernel_build);
    }

    if (option_machine || option_all)
    {
        printf("%s ", __BUILD_TARGET__);
    }

    if (option_operating_system || option_all)
    {
        printf("%s", info.system_name);
    }

    printf("\n");

    return PROCESS_FAILURE;
}
