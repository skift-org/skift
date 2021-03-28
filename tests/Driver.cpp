#include <libio/Streams.h>
#include <libsystem/Logger.h>
#include <libutils/Vector.h>

#include "tests/Driver.h"

namespace Test
{

static Vector<Test> *_tests;

void __register_test(Test &test)
{
    if (!_tests)
    {
        _tests = new Vector<Test>();
    }

    _tests->push_back(test);
}

const char *_good_messages[] = {
    "I see this as an absolute win.",
    "Ca c'est pixel",
    "Pro gamer moment",
    "I love C++",
    "You should be proud",
    "Thanks stack-overflow",
    "Nice",
    "Good",
    "MLG move",
    "based",
    "Now do a git push",
    "Now write more tests",
};

const char *_bad_messages[] = {
    "bruh",
    "oof",
    "PC LOAD LETTER",
    "That's not a success",
    "Minecraft crashed!",
    "Quite honestly, I wouldn't worry myself about that.",
    "This doesn't make any sense!",
    "It's not a good surprise...",
    "I should have writen thats in rust",
    "It's the kernel fault",
    "It's the compiler fault",
    "It's monax's fault",
    "Ask on the discord",
    "I hate C++",
    "Typo in the code.",
    "Uh... Did I do that?",
    "Don't do that.",
    "Just read the wiki",
    "wiki.osdev.org",
    "osdev.wiki",
    "OOF!",
    "OoooOOoOoOF!",
    "At least the kernel didn't panic",
    "But it works on my machine",
};

int run_all_testes()
{
    assert(_tests);

    IO::errln("test: Running {} tests\n", _tests->count());

    int passed = 0;
    int failed = 0;

    uint32_t start_tick = 0;
    hj_system_tick(&start_tick);

    for (auto &test : *_tests)
    {
        IO::err("test: {}: \e[1m{}\e[m... ", test.location.file(), test.name);

        int child_pid = -1;
        hj_process_clone(&child_pid, TASK_WAITABLE);

        if (child_pid == 0)
        {
            test.function();
            hj_process_exit(PROCESS_SUCCESS);
        }

        int child_result = PROCESS_FAILURE;
        process_wait(child_pid, &child_result);

        if (child_result == PROCESS_SUCCESS)
        {
            IO::errln("\e[1;32mok\e[m");
            passed++;
        }
        else
        {
            IO::errln("\e[1;31mfailed\e[m");
            failed++;
        }
    }

    uint32_t end_tick = 0;
    hj_system_tick(&end_tick);

    IO::errln("");

    if (failed == 0)
    {
        IO::errln("\e[90m// {}\e[m", _good_messages[end_tick % __array_length(_good_messages)]);
    }
    else
    {
        IO::errln("\e[90m// {}\e[m", _bad_messages[end_tick % __array_length(_bad_messages)]);
    }

    IO::errln("");
    IO::errln("test: Took \e[1m{}ms\e[m", end_tick - start_tick);
    IO::errln("test: Passed \e[1;32m{}\e[m", passed);
    IO::errln("test: Failled \e[1;31m{}\e[m", failed);

    if (failed == 0)
    {
        IO::logln("test: PASSED");
    }
    else
    {
        IO::logln("test: FAILED");
    }

    return 0;
}

} // namespace Test
