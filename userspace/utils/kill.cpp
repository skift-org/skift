#include <libio/File.h>
#include <libjson/Json.h>
#include <libshell/ArgParse.h>
#include <libsystem/process/Process.h>

int kill(int pid)
{
    if (process_cancel(pid) != SUCCESS)
    {
        return PROCESS_FAILURE;
    }

    return PROCESS_SUCCESS;
}

int killall(String name)
{
    IO::File file{"/System/processes", HJ_OPEN_READ};

    if (!file.exist())
    {
        IO::errln("/System/processes not found");
        return PROCESS_FAILURE;
    }

    if (name == "neko")
    {
        IO::errln("Don't kill nekos, your are a bad persone!");
    }

    if (name == "cat")
    {
        IO::errln("Don't kill cats, you monster!");
    }

    auto processes = Json::parse(file);

    for (size_t i = 0; i < processes.length(); i++)
    {
        const auto &proc = processes.get(i);

        if (proc.get("name").as_string() == name)
        {
            kill(proc.get("id").as_integer());
        }
    }

    return PROCESS_SUCCESS;
}

int main(int argc, char const *argv[])
{
    Shell::ArgParse args;

    args.show_help_if_no_operand_given();
    args.should_abort_on_failure();

    bool option_all;
    args.option(option_all, 'a', "all", "Kill all processes with a matching name.");

    auto parse_result = args.eval(argc, argv);
    if (parse_result != Shell::ArgParseResult::SHOULD_CONTINUE)
    {
        return parse_result == Shell::ArgParseResult::SHOULD_FINISH
                   ? PROCESS_SUCCESS
                   : PROCESS_FAILURE;
    }

    if (option_all)
    {
        for (size_t i = 0; i < args.argc(); i++)
        {
            if (killall(args.argv()[i]) != PROCESS_SUCCESS)
            {
                return PROCESS_FAILURE;
            }
        }
    }
    else
    {
        for (size_t i = 0; i < args.argc(); i++)
        {
            if (kill(atoi(args.argv()[i].cstring())) != PROCESS_SUCCESS)
            {
                return PROCESS_FAILURE;
            }
        }
    }

    return PROCESS_SUCCESS;
}
