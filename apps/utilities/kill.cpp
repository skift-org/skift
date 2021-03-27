#include <libsystem/process/Process.h>
#include <libutils/ArgParse.h>
#include <libutils/NumberParser.h>
#include <libutils/json/Json.h>

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
    if (name == "neko")
    {
        IO::errln("Don't kill nekos, your are a bad persone!");
    }

    if (name == "cat")
    {
        IO::errln("Don't kill cats, you monster!");
    }

    auto processes = Json::parse_file("/System/processes");

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
    ArgParse args;

    args.show_help_if_no_operand_given();
    args.should_abort_on_failure();

    bool option_all;
    args.option(option_all, 'a', "all", "Kill all processes with a matching name.");

    args.eval(argc, argv);

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
            int pid = parse_uint_inline(PARSER_DECIMAL, args.argv()[i].cstring(), -1);

            if (kill(pid) != PROCESS_SUCCESS)
            {
                return PROCESS_FAILURE;
            }
        }
    }

    return PROCESS_SUCCESS;
}
