#include <abi/Syscalls.h>

#include <libio/Streams.h>
#include <libsystem/BuildInfo.h>
#include <libsystem/system/System.h>
#include <libutils/ArgParse.h>

constexpr auto PROLOGUE = "Print certain system information.  With no OPTION, same as -s.";

constexpr auto OPTION_ALL_DESCRIPTION = "Print the kernel name.";
constexpr auto OPTION_KERNEL_NAME_DESCRIPTION = "Print the kernel name.";
constexpr auto OPTION_KERNEL_RELEASE_DESCRIPTION = "Print the kernel release.";
constexpr auto OPTION_KERNEL_VERSION_DESCRIPTION = "Print the kernel version.";
constexpr auto OPTION_MACHINE_DESCRIPTION = "Print the machine hardware name.";
constexpr auto OPTION_NODENAME_DESCRIPTION = "Print the network node hostname.";
constexpr auto OPTION_OPERATING_SYSTEM_DESCRIPTION = "Print the operating system.";

constexpr auto EPILOGUE = "Options can be combined";

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

    SystemInfo info{};
    hj_system_info(&info);

    if (argc == 1 || option_kernel_name || option_all)
    {
        IO::out("{} ", info.kernel_name);
    }

    if (option_nodename || option_all)
    {
        IO::out("{} ", info.machine);
    }

    if (option_kernel_release || option_all)
    {
        IO::out("{} ", info.kernel_release);
    }

    if (option_kernel_version || option_all)
    {
        IO::out("{} ", info.kernel_build);
    }

    if (option_machine || option_all)
    {
        IO::out("{} ", __BUILD_TARGET__);
    }

    if (option_operating_system || option_all)
    {
        IO::out("{} ", info.system_name);
    }

    IO::out("\n");

    return PROCESS_SUCCESS;
}
