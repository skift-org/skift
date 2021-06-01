#include <stdio.h>

#include "archs/Arch.h"

#include "system/Streams.h"
#include "system/graphics/EarlyConsole.h"
#include "system/graphics/Font.h"
#include "system/interrupts/Interupts.h"
#include "system/scheduling/Scheduler.h"
#include "system/system/System.h"
#include "system/tasking/Task.h"

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

void system_panic_internal(SourceLocation location, void *stackframe, const char *message, ...)
{
    interrupts_retain();
    interrupts_disable_holding();

    font_set_bg(0xff171717);
    font_set_fg(0xffFAFAFA);

    early_console_enable();

    va_list va;
    va_start(va, message);

    if (nested_panic)
    {
        system_stop();
    }

    Kernel::logln("\n");

    if (!has_panic)
    {
        has_panic = true;

        Kernel::logln("--- !!! ------------------------------------------------------------------------");
        Kernel::logln("");
        Kernel::logln("\tKERNEL PANIC");
        Kernel::logln("\t// {}", witty_comments[system_get_tick() % ARRAY_LENGTH(witty_comments)]);
    }
    else
    {
        nested_panic = true;

        Kernel::logln("- - NESTED - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
        Kernel::logln("");
        Kernel::logln("\tNESTED KERNEL PANIC", YO_DAWG);
        Kernel::logln("\t// {}", YO_DAWG);
    }

    char buffer[256];
    vsnprintf(buffer, 256, message, va);
    Kernel::logln(buffer);

    Kernel::logln("\tthrow by {} {}() ln{}", location.file(), location.function(), location.line());
    Kernel::logln("\tThe system was running for {} tick.", system_get_tick());

    if (scheduler_running_id() != -1)
    {
        Kernel::logln("\tThe running process is {}: {}", scheduler_running_id(), scheduler_running()->name);
    }

    if (scheduler_is_context_switch())
    {
        Kernel::logln("\tWe are context switching", system_get_tick());
    }

    Kernel::logln("");

    Kernel::logln("\tStackframe:");
    if (stackframe)
    {
        Arch::dump_stack_frame(stackframe);
    }
    else
    {
        Arch::backtrace();
    }
    Kernel::logln("");

    memory_dump();

    Kernel::logln("");

    if (!nested_panic)
    {
        task_dump(scheduler_running());
    }

    Kernel::logln("");
    Kernel::logln("\tSystem halted!");
    Kernel::logln("");

    Kernel::logln("--------------------------------------------------------------------------------");
    Kernel::logln("");

    system_stop();
}
