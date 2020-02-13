/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libdevice/keymap.h>

static KeyMapping *keymap_lookup(KeyMap *this, Key key)
{
    for (int i = 0; i < this->mappings_count; i++)
    {
        if (this->mappings[i].key == key)
        {
            return &this->mappings[i];
        }
    }

    return NULL;
}