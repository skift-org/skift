#include <abi/Syscalls.h>

#include <libshell/ArgParse.h>

int main(int argc, char const *argv[])
{
    Shell::ArgParse args;

    args.should_abort_on_failure();
    args.show_help_if_no_option_given();

    args.prologue("Change the power state of the system.");

    args.option('r', "reboot", "Reboot the system.", [&](auto &) {
        hj_system_reboot();
        return Shell::ArgParseResult::SHOULD_FINISH;
    });

    args.option('s', "shutdown", "Shutdown the system.", [&](auto &) {
        hj_system_shutdown();
        return Shell::ArgParseResult::SHOULD_FINISH;
    });

    return args.eval(argc, argv) == Shell::ArgParseResult::FAILURE
               ? PROCESS_FAILURE
               : PROCESS_SUCCESS;
}
