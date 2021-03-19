#include <libsystem/cmdline/History.h>
#include <libsystem/utils/List.h>
#include <libtest/AssertLowerThan.h>

static List *history = nullptr;

static void initialize_history_if_not_already()
{
    if (history)
    {
        return;
    }

    history = list_create();
}

void history_commit(UnicodeString *text)
{
    if (unicode_string_length(text) == 0)
    {
        return;
    }

    initialize_history_if_not_already();

    if (history->any() &&
        unicode_string_equals(history_peek(0), text))
    {
        // We don't want duplicated entry in our history!
        return;
    }

    list_push(history, unicode_string_clone(text));
}

UnicodeString *history_peek(size_t index)
{
    initialize_history_if_not_already();

    assert_lower_than(index, history_length());

    UnicodeString *str = nullptr;
    list_peekat(history, index, (void **)&str);

    return str;
}

size_t history_length()
{
    initialize_history_if_not_already();

    return history->count();
}
