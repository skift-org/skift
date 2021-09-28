#include <abi/Syscalls.h>

#include <libio/Streams.h>

int main(int argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    SystemInfo info{};
    hj_system_info(&info);

    IO::out("\e[1;94m");
    IO::out("       ___      \n");
    IO::out("      /\\  \\     \n");
    IO::out("     /::\\  \\    \n");
    IO::out("    /:/\\ \\  \\   \n");
    IO::out("   _\\:\\~\\ \\  \\  \n");
    IO::out("  /\\ \\:\\ \\ \\__\\ \n");
    IO::out("  \\:\\ \\:\\ \\/__/ \n");
    IO::out("   \\:\\ \\:\\__\\   \n");
    IO::out("    \\:\\/:/  /   \n");
    IO::out("     \\::/  /    \n");
    IO::out("      \\/__/     \n");
    IO::out("\e[m");

    IO::out("\e[11A");
    IO::outln("\e[16C user@{}", info.machine);
    IO::outln("\e[16C ------------");
    IO::outln("\e[16C OS: {}OS", info.system_name);
    IO::outln("\e[16C KERNEL: {}", info.kernel_name);
    IO::outln("\e[16C VERSION: {}", info.kernel_release);
    IO::outln("\e[16C BUILD: {}", info.kernel_build);
    IO::out("\e[16C UPTIME: ");

    SystemStatus status;
    hj_system_status(&status);

    ElapsedTime seconds = status.uptime;

    if (seconds / 86400 > 0)
    {
        IO::out("{} day{}, ", seconds / 86400, (seconds / 86400) == 1 ? "" : "s");
        seconds %= 86400;
    }

    if (seconds / 3600 > 0)
    {
        IO::out("{} hour{}, ", seconds / 3600, (seconds / 3600) == 1 ? "" : "s");
        seconds %= 3600;
    }

    if (seconds / 60 > 0)
    {
        IO::out("{} minute{}, ", seconds / 60, (seconds / 60) == 1 ? "" : "s");
        seconds %= 60;
    }

    IO::out("{} second{}\n", seconds, seconds == 1 ? "" : "s");

    IO::out("\e[16C SHELL: /Applications/shell\n");
    IO::out("\e[16C TERMINAL: /Applications/terminal\n");
    IO::out("\e[16C COMPOSITOR: /Applications/compositor\n");
    IO::out("\e[16C MEMORY: \e[m{}Mib / {}Mib\n", (int)(status.used_ram / (1024 * 1024)), (int)(status.total_ram / (1024 * 1024)));

    IO::out("\n");
    IO::out("\e[16C \e[40m  \e[41m  \e[42m  \e[43m  \e[44m  \e[45m  \e[46m  \e[47m  \e[m\n");

    return PROCESS_SUCCESS;
}
