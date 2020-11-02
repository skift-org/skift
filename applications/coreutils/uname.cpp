#include <libsystem/BuildInfo.h>
#include <libsystem/system/System.h>
#include <libutils/ArgParse.h>

constexpr ConstString PROLOGUE = "Print certain system information.  With no OPTION, same as -s.";

constexpr ConstString OPTION_ALL_DESCRIPTION = "Print the kernel name.";
constexpr ConstString OPTION_KERNEL_NAME_DESCRIPTION = "Print the kernel name.";
constexpr ConstString OPTION_KERNEL_RELEASE_DESCRIPTION = "Print the kernel release.";
constexpr ConstString OPTION_KERNEL_VERSION_DESCRIPTION = "Print the kernel version.";
constexpr ConstString OPTION_MACHINE_DESCRIPTION = "Print the machine hardware name.";
constexpr ConstString OPTION_NODENAME_DESCRIPTION = "Print the network node hostname.";
constexpr ConstString OPTION_OPERATING_SYSTEM_DESCRIPTION = "Print the operating system.";

constexpr ConstString EPILOGUE = "Options can be combined";

int main(int argc, char const *argv[])
{

    ArgParse args;
    args.should_abort_on_failure();

    args.prologue(PROLOGUE);

    args.usage("");
    args.usage("OPTION...");

    bool option_all = false;
    args.option(option_all, 'a', "all", OPTION_ALL_DESCRIPTION);

    bool option_kernel_name = false;
    args.option(option_kernel_name, 's', "kernel-name", OPTION_KERNEL_NAME_DESCRIPTION);

    bool option_kernel_release = false;
    args.option(option_kernel_release, 'r', "kernel-release", OPTION_KERNEL_RELEASE_DESCRIPTION);

    bool option_kernel_version = false;
    args.option(option_kernel_version, 'v', "kernel-version", OPTION_KERNEL_VERSION_DESCRIPTION);

    bool option_machine = false;
    args.option(option_machine, 'm', "machine", OPTION_MACHINE_DESCRIPTION);

    bool option_nodename = false;
    args.option(option_nodename, 'n', "nodename", OPTION_NODENAME_DESCRIPTION);

    bool option_operating_system = false;
    args.option(option_operating_system, 'o', "operating-system", OPTION_OPERATING_SYSTEM_DESCRIPTION);

    args.epiloge(EPILOGUE);

    args.eval(argc, argv);

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

    return PROCESS_SUCCESS;
}
