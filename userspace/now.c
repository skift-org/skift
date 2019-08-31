#include <libsystem/iostream.h>
#include <libsystem/time.h>
#include <libsystem/cmdline.h>

static bool option_time = false;
static bool option_date = false;
static bool option_epoch = false;

static const char *usages[] = {
    "",
    "OPTION...",
    NULL};

static cmdline_option_t options[] = {
    CMDLINE_OPT_HELP,

    CMDLINE_OPT_BOOL("time", 't', option_time, "Display the time of the day.", CMDLINE_NO_CALLBACK),
    CMDLINE_OPT_BOOL("date", 'd', option_date, "Display today date.", CMDLINE_NO_CALLBACK),
    CMDLINE_OPT_BOOL("epoch", 'e', option_epoch, "Display the current epoch.", CMDLINE_NO_CALLBACK),

    CMDLINE_OPT_END,
};

static cmdline_t cmdline = CMDLINE(usages, options, "Get the time of the day.", "Options can be combined.");

int main(int argc, char **argv)
{
    argc = cmdline_parse(&cmdline, argc, argv);

    if (!(option_time || option_date || option_epoch))
    {
        option_time = true;
        option_date = true;
        option_epoch = false;
    }

    timestamp_t timestamp = timestamp_now();
    datetime_t datetime = timestamp_to_datetime(timestamp);

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

    return 0;
}
