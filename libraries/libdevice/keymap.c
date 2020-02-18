/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libdevice/keymap.h>

static KeyMapping *keymap_lookup(KeyMap *keymap, Key key)
{
    for (int i = 0; i < keymap->mappings_count; i++)
    {
        if (keymap->mappings[i].key == key)
        {
            return &keymap->mappings[i];
        }
    }

    return NULL;
}