/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libdevice/keys.h>

#define KEY_NAMES_ENTRY(__key_name, __key_number) [__key_number] = #__key_name,

static const char *KEYS_NAMES[] = {KEY_LIST(KEY_NAMES_ENTRY)};

const char *key_to_string(Key key)
{
    if (key < __KEY_COUNT)
    {
        return KEYS_NAMES[key];
    }
    else
    {
        return "overflow";
    }
}

bool key_is_valid(Key key)
{
    return key > 0 && key < __KEY_COUNT;
}