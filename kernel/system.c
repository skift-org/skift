/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/atomic.h>

#include "x86/cpuid.h"
#include "system.h"
#include "tasking.h"
#include "memory.h"

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
    "Surprise! Haha. Well, this is awkward.",
    "Oh - I know what I did wrong!",
    "Uh... Did I do that?",
    "Oops.",
    "On the bright side, I bought you a teddy bear!",
    "Yo DAWG, I heard you like errors,\n\t// so i put an error in your error handler\n\t// so you can get error while you get error",
    "Excuse me Sir, \n\t// Do you have a moment to talk aboucrash reports, originally exclusively fot TempleOS?",
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
    "Get the f*** outa my room, I'm playing minecraft"};

static bool has_panic = false;
static bool nested_panic = false;

void __panic(const char *package, const char *file, const char *function, const int line, processor_context_t *context, const char *message, ...)
{
    atomic_begin();

    va_list va;
    va_start(va, message);

    if (!has_panic)
    {
        printf("\n\033[0;33m--- \033[0;31m!!!\033[0;33m ------------------------------------------------------------------------\033[0m\n");
        printf("\n\tKERNEL");
    }
    else
    {
        nested_panic = true;
        printf("\n\n\033[0;33m- - \033[0;31mNESTED\033[0;33m - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\033[0m\n");
        printf("\n\tNESTED");
    }

    printf(" PANIC\n\t// %s\n\n\t\033[0;31m", witty_comments[sheduler_get_ticks() % (sizeof(witty_comments) / sizeof(char *))]);

    has_panic = true;

    vprintf(message, va);
    printf("\033[0m\n\tthrow by %s::%s %s() ln%d", package, file, function, line);

    printf("\n");
    printf("\n\tDiagnostic:");
    printf("\n\tThe system was running for %d tick.", sheduler_get_ticks());
    printf("\n\tThe running process is %d: %s", sheduler_running_id(), sheduler_running()->name);

    if (sheduler_is_context_switch())
    {
        printf("\n\tWe are context switching\n", sheduler_get_ticks());
    }
    else
    {
        printf("\n");
    }

    if (context != NULL)
    {
        printf("\n\tContext:\n");
        processor_dump_context(context);
    }

    memory_dump();

    if (!nested_panic)
    {
        // filesystem_panic_dump();
        // task_panic_dump();
        // cpuid_dump();
    }

    printf("\n");

    puts("\n\tSystem halted!\n");

    printf("\n\033[0;33m--------------------------------------------------------------------------------");

    STOP;
}