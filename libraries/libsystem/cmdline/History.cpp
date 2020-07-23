#include <libsystem/Assert.h>
#include <libsystem/cmdline/History.h>
#include <libsystem/utils/List.h>

static List *history = NULL;

static void initialize_history_if_not_already(void)
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

    if (!list_empty(history) &&
        unicode_string_equals(history_peek(0), text))
    {
        // We don't went duplicated entry in our history!
        return;
    }

    list_push(history, unicode_string_clone(text));
}

UnicodeString *history_peek(size_t index)
{
    initialize_history_if_not_already();

    assert(index < history_length());

    UnicodeString *str = NULL;
    list_peekat(history, index, (void **)&str);

    return str;
}

size_t history_length(void)
{
    initialize_history_if_not_already();

    return list_count(history);
}
