/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <stdarg.h>
#include <stdlib.h>
#include "kernel/system.h"
#include "kernel/tasking.h"
#include "sync/atomic.h"

const char *const witty_comments[] =
    {
        "Witty comment unavailable :(",
        "Surprise! Haha. Well, this is awkward.",
        "Oh - I know what I did wrong!",
        "Uh... Did I do that?",
        "Oops.",
        "On the bright side, I bought you a teddy bear!",
        "Yo DAWG, I heard you like errors,\n\t// so i put an error in your error handler\n\t// so you can error while you error",
        "Excuse Me Sir, \n\t// Do You Have a Moment to Talk About Jesus Christ?",
        "DON'T PANIC!",
        "...",

        "Greenpeace free'd the mallocs \\o/",
        "Typo in the code.",
        "System consumed all the paper for paging!",
        "Suspicious pointer corrupted the machine."};

extern uint ticks;

void __panic(const string file, const string function, const int line, context_t *context, string message, ...)
{

    cli();
    atomic_disable();

    va_list va;
    va_start(va, message);

    printf("\n--- !!! ------------------------------------------------------------------------\n");

    printf("\n\tKERNEL PANIC\n\t// %s\n\n\t", witty_comments[ticks % (9 + 4)]);

    vprintf(message, va);
    printf("\n\tat %s %s() ln%d", file, function, line);

    printf("\n");
    printf("\n\tDiagnostic:");
    printf("\n\tThe system was running for %d tick.", ticks);
    // thread_dump(thread_self());
    printf("\n");

    // thread_dump_all();
    printf("\n\n");

    if (context != NULL)
    {
        dump_context(context);
    }

    puts("\n\tSystem halted!\n");

    STOP;
}