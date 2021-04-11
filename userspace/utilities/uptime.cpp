#include <abi/Syscalls.h>
#include <libio/Streams.h>

int main(int argc, char const *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    SystemStatus status{};
    hj_system_status(&status);

    ElapsedTime seconds = status.uptime;

    IO::out("Up ");

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

    return PROCESS_SUCCESS;
}
