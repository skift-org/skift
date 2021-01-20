#include "archs/Architectures.h"

#include "kernel/graphics/EarlyConsole.h"
#include "kernel/graphics/Font.h"
#include "kernel/interrupts/Interupts.h"
#include "kernel/scheduling/Scheduler.h"
#include "kernel/system/System.h"
#include "kernel/tasking/Task.h"

static const char *YO_DAWG =
    "Yo DAWG, I heard you like errors, \n\t"
    "// so I put an error in your error handler\n\t"
    "// so you can get error while you get error";

static const char *const witty_comments[] = {
    "Witty comment unavailable :(",

    "Excuse me Sir, \n\t"
    "// Do you have a moment to talk about TempleOS?",

    "Surprise! Haha. Well, this is awkward.",
    "Oh - I know what I did wrong!",
    "Uh... Did I do that?",
    "Oops.",
    "On the bright side, I bought you a teddy bear!",
    "DON'T PANIC!",
    "...",
    "Greenpeace free'd the mallocs \\o/",
    "Typo in the code.",
    "System consumed all the paper for paging!",
    "Suspicious pointer corrupted the machine.",
    "I'm tired of this ;_;",
    "PC LOAD LETTER",
    "Abort, Retry, Fail?",
    "Bad command or file name",
    "OOF!",
    "OoooOOoOoOF!",
    "Et là c'est le drame...",
    "Everything's going to plan. No, really, that was supposed to happen.",
    "My bad.",
    "Minecraft crashed!",
    "Quite honestly, I wouldn't worry myself about that.",
    "This doesn't make any sense!",
    "It's not a good surprise...",
    "Don't do that.",
    "Get the f*** outa my room, I'm playing minecraft",
};

static bool has_panic = false;
static bool nested_panic = false;

void system_panic_internal(__SOURCE_LOCATION__ location, void *stackframe, const char *message, ...)
{
    interrupts_retain();
    interrupts_disable_holding();

    font_set_bg(0xff333333);

    early_console_enable();

    va_list va;
    va_start(va, message);

    if (nested_panic)
    {
        system_stop();
    }

    if (!has_panic)
    {
        has_panic = true;
        printf("\n\e[0;33m--- \e[0;31m!!!\e[0;33m ------------------------------------------------------------------------\e[0m\n");
        printf("\n\tKERNEL");
        printf(" PANIC\n\t// %s\n\n\t\e[0;31m", witty_comments[system_get_tick() % __array_length(witty_comments)]);
    }
    else
    {
        nested_panic = true;
        printf("\n\n\e[0;33m- - \e[0;31mNESTED\e[0;33m - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\e[0m\n");
        printf("\n\tNESTED");
        printf(" PANIC\n\t// %s\n\n\t\e[0;31m", YO_DAWG);
    }

    vprintf(message, va);
    printf("\e[0m\n\tthrow by %s %s() ln%d", location.file, location.function, location.line);

    printf("\n");
    printf("\n\tDiagnostic:");
    printf("\n\tThe system was running for %d tick.", system_get_tick());

    if (scheduler_running_id() != -1)
    {
        printf("\n\tThe running process is %d: %s", scheduler_running_id(), scheduler_running()->name);
    }

    if (scheduler_is_context_switch())
    {
        printf("\n\tWe are context switching\n", system_get_tick());
    }
    else
    {
        printf("\n");
    }

    printf("\n\tStackframe:\n");
    if (stackframe)
    {
        arch_dump_stack_frame(stackframe);
    }
    else
    {
        arch_backtrace();
    }
    printf("\n");

    memory_dump();

    printf("\n");

    if (!nested_panic)
    {
        task_dump(scheduler_running());
        arch_panic_dump();
    }

    printf("\n");

    printf("\n\tSystem halted!\n");

    printf("\n\e[0;33m--------------------------------------------------------------------------------\n\n");

    system_stop();
}
