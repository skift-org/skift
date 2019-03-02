/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <stdarg.h>
#include <stdlib.h>
#include <skift/atomic.h>

#include "kernel/tasking.h"
#include "kernel/system.h"

const char *const witty_comments[] =
{
    "Witty comment unavailable :(",
    "Surprise! Haha. Well, this is awkward.",
    "Oh - I know what I did wrong!",
    "Uh... Did I do that?",
    "Oops.",
    "On the bright side, I bought you a teddy bear!",
    "Yo DAWG, I heard you like errors,\n\t// so i put an error in your error handler\n\t// so you can get error while you get error",
    "Excuse me Sir, \n\t// Do you have a moment to talk about TempleOS?",
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
};

extern uint ticks;
extern bool is_context_switch;

void __panic(const char* package, const char* file, const char* function, const int line, processor_context_t * context, string message, ...)
{
    sk_atomic_begin();

    va_list va;
    va_start(va, message);

    printf("\n\033[0;33m--- \033[0;31m!!!\033[0;33m ------------------------------------------------------------------------\033[0m\n");

    printf("\n\tKERNEL PANIC\n\t// %s\n\n\t\033[0;31m", witty_comments[ticks % (sizeof(witty_comments)/sizeof(char*))]);

    vprintf(message, va);
    printf("\033[0m\n\tat %s::%s %s() ln%d",package, file, function, line);

    printf("\n");
    printf("\n\tDiagnostic:");
    printf("\n\tThe system was running for %d tick.", ticks);
    
    if (is_context_switch)
    {
        printf("\n\tWe are context switching\n", ticks);
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

    //thread_dump_all();

    printf("\n");

    puts("\n\tSystem halted!\n");

    printf("\n\033[0;33m--------------------------------------------------------------------------------");

    STOP;
}
