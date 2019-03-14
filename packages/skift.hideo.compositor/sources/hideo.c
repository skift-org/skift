#include <skift/cmdline.h>

#include "hideo/compositor.h"

static const char *usages[] = 
{
    "",
    NULL
};

static cmdline_option_t options[] = 
{
    CMDLINE_OPT_HELP,

    CMDLINE_OPT_END,
};

static cmdline_t cmdline = CMDLINE(usages, options, "Start a new graphical session.", "Options can be combined.");

int main(int argc, char *argv[])
{
    argc = cmdline_parse(&cmdline, argc, argv);

    return 0;
}
