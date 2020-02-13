/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/atomic.h>

#include "kernel/memory.h"
#include "kernel/system.h"
#include "kernel/tasking.h"
#include "kernel/x86/cpuid.h"

/* --- Version info --------------------------------------------------------- */

char *__kernel_name = "hjert";

int __kernel_version_major = 0;
int __kernel_version_minor = 2;
int __kernel_version_patch = 0;
char *__kernel_version_codename = "wheat";

/* kernel version format major.minor.patch-codename */
char *__kernel_version_format = "%d.%d.%d-%s @ " __COMMIT__;
char *__kernel_uname_format = "%s %d.%d.%d-%s @ " __COMMIT__;

/* --- Panic screen --------------------------------------------------------- */

const char *const witty_comments[] = {
    "Witty comment unavailable :(",

    "Yo DAWG, I heard you like errors, \n\t"
    "// so I put an error in your error handler\n\t"
    "// so you can get error while you get error",

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

void __panic(const char *file, const char *function, const int line, InterruptStackFrame *stackframe, const char *message, ...)
{
    atomic_begin();
    atomic_disable();

    va_list va;
    va_start(va, message);

    if (nested_panic)
    {
        STOP;
    }

    if (!has_panic)
    {
        has_panic = true;
        printf("\n\e[0;33m--- \e[0;31m!!!\e[0;33m ------------------------------------------------------------------------\e[0m\n");
        printf("\n\tKERNEL");
    }
    else
    {
        nested_panic = true;
        printf("\n\n\e[0;33m- - \e[0;31mNESTED\e[0;33m - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\e[0m\n");
        printf("\n\tNESTED");
    }

    printf(" PANIC\n\t// %s\n\n\t\e[0;31m", witty_comments[sheduler_get_ticks() % (sizeof(witty_comments) / sizeof(char *))]);

    vprintf(message, va);
    printf("\e[0m\n\tthrow by %s %s() ln%d", file, function, line);

    printf("\n");
    printf("\n\tDiagnostic:");
    printf("\n\tThe system was running for %d tick.", sheduler_get_ticks());

    if (sheduler_running_id() != -1)
    {
        printf("\n\tThe running process is %d: %s", sheduler_running_id(), sheduler_running()->name);
    }

    if (sheduler_is_context_switch())
    {
        printf("\n\tWe are context switching\n", sheduler_get_ticks());
    }
    else
    {
        printf("\n");
    }

    if (stackframe)
    {
        printf("\n\tContext:\n");
        interrupts_dump_stackframe(stackframe);
    }

    memory_dump();

    printf("\n");

    if (!nested_panic)
    {
        task_panic_dump();
        cpuid_dump();
    }

    printf("\n");

    printf("\n\tSystem halted!\n");

    printf("\n\e[0;33m--------------------------------------------------------------------------------\n\n");

    STOP;
}