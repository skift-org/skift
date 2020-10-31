#include <libsystem/BuildInfo.h>
#include <libsystem/system/System.h>
#include <libutils/ArgParse.h>

constexpr ConstString PROLOGUE = "Print certain system information.  With no OPTION, same as -s.";
constexpr ConstString EPILOGUE = "Options can be combined";
constexpr ConstString OPTION_ALL_DESCRIPTION = "Print the kernel name.";
constexpr ConstString OPTION_KERNEL_NAME_DESCRIPTION = "Print the kernel name.";
constexpr ConstString OPTION_KERNEL_RELEASE_DESCRIPTION = "Print the kernel release.";
constexpr ConstString OPTION_KERNEL_VERSION_DESCRIPTION = "Print the kernel version.";
constexpr ConstString OPTION_MACHINE_DESCRIPTION = "Print the machine hardware name.";
constexpr ConstString OPTION_NODENAME_DESCRIPTION = "Print the network node hostname.";
constexpr ConstString OPTION_OPERATING_SYSTEM_DESCRIPTION = "Print the operating system.";

int main(int argc, char const *argv[])
{
    ArgParse args;
    args.should_abort_on_failure();

    args.prologue(PROLOGUE);

    args.usage("");
    args.usage("OPTION...");

    auto option_all = args.option<OptionBool>('a');
    option_all->longname("all");
    option_all->description(OPTION_ALL_DESCRIPTION);

    auto option_kernel_name = args.option<OptionBool>('s');
    option_kernel_name->longname("kernel-name");
    option_kernel_name->description(OPTION_KERNEL_NAME_DESCRIPTION);

    auto option_kernel_release = args.option<OptionBool>('r');
    option_kernel_release->longname("kernel-release");
    option_kernel_release->description(OPTION_KERNEL_RELEASE_DESCRIPTION);

    auto option_kernel_version = args.option<OptionBool>('v');
    option_kernel_version->longname("kernel-version");
    option_kernel_version->description(OPTION_KERNEL_VERSION_DESCRIPTION);

    auto option_machine = args.option<OptionBool>('m');
    option_machine->longname("machine");
    option_machine->description(OPTION_MACHINE_DESCRIPTION);

    auto option_nodename = args.option<OptionBool>('n');
    option_nodename->longname("nodename");
    option_nodename->description(OPTION_NODENAME_DESCRIPTION);

    auto option_operating_system = args.option<OptionBool>('o');
    option_operating_system->longname("operating-system");
    option_operating_system->description(OPTION_OPERATING_SYSTEM_DESCRIPTION);

    args.epiloge(EPILOGUE);

    args.eval(argc, argv);

    SystemInfo info = system_get_info();

    if (argc == 1 || option_kernel_name() || option_all())
    {
        printf("%s ", info.kernel_name);
    }

    if (option_nodename() || option_all())
    {
        printf("%s ", info.machine);
    }

    if (option_kernel_release() || option_all())
    {
        printf("%s ", info.kernel_release);
    }

    if (option_kernel_version() || option_all())
    {
        printf("%s ", info.kernel_build);
    }

    if (option_machine() || option_all())
    {
        printf("%s ", __BUILD_TARGET__);
    }

    if (option_operating_system() || option_all())
    {
        printf("%s", info.system_name);
    }

    printf("\n");

    return PROCESS_SUCCESS;
}
