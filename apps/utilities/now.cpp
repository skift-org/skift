#include <libsystem/cmdline/CMDLine.h>
#include <libsystem/io/Stream.h>
#include <skift/Time.h>

#include <stdio.h>

static bool option_time = false;
static bool option_date = false;
static bool option_epoch = false;

static const char *usages[] = {
    "",
    "OPTION...",
    nullptr};

static CommandLineOption options[] = {
    COMMANDLINE_OPT_HELP,

    COMMANDLINE_OPT_BOOL("time", 't', option_time, "Display the time of the day.", COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_BOOL("date", 'd', option_date, "Display today date.", COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_BOOL("epoch", 'e', option_epoch, "Display the current epoch.", COMMANDLINE_NO_CALLBACK),

    COMMANDLINE_OPT_END,
};

static CommandLine cmdline = CMDLINE(
    usages,
    options,
    "Get the time of the day.",
    "Options can be combined.");

int main(int argc, char **argv)
{
    argc = cmdline_parse(&cmdline, argc, argv);

    if (!(option_time || option_date || option_epoch))
    {
        option_time = true;
        option_date = true;
        option_epoch = false;
    }

    TimeStamp timestamp = timestamp_now();
    DateTime datetime = timestamp_to_datetime(timestamp);

    if (option_time)
    {
        printf("%d:%d:%d ", datetime.hour, datetime.minute, datetime.second);
    }

    if (option_date)
    {
        printf("%d/%d/%d ", datetime.day, datetime.month, datetime.year);
    }

    if (option_epoch)
    {
        printf("%d", timestamp);
    }

    printf("\n");

    return PROCESS_SUCCESS;
}
