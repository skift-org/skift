#include <skift/Time.h>

#include <libio/Streams.h>
#include <libshell/ArgParse.h>

int main(int argc, const char *argv[])
{
    Shell::ArgParse args;

    args.prologue("Get the time of the day.");
    args.usage("");
    args.usage("OPTION...");

    bool option_time = false;
    args.option(option_time, 't', "time", "Display the time of the day.");

    bool option_date = false;
    args.option(option_date, 'd', "date", "Display today date.");

    bool option_epoch = false;
    args.option(option_epoch, 'e', "epoch", "Display the current epoch.");

    args.epiloge("Options can be combined.");

    auto parse_result = args.eval(argc, argv);
    if (parse_result != Shell::ArgParseResult::SHOULD_CONTINUE)
    {
        return parse_result == Shell::ArgParseResult::SHOULD_FINISH
                   ? PROCESS_SUCCESS
                   : PROCESS_FAILURE;
    }

    if (!option_time && !option_date && !option_epoch)
    {
        option_time = true;
        option_date = true;
        option_epoch = false;
    }

    TimeStamp timestamp = timestamp_now();
    DateTime datetime = timestamp_to_datetime(timestamp);

    if (option_time)
    {
        IO::out("{}:{}:{} ", datetime.hour, datetime.minute, datetime.second);
    }

    if (option_date)
    {
        IO::out("{}/{}/{} ", datetime.day, datetime.month, datetime.year);
    }

    if (option_epoch)
    {
        IO::out("{}", timestamp);
    }

    IO::out("\n");

    return PROCESS_SUCCESS;
}
