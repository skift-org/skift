
#include <libdevice/keymap.h>

KeyMapping *keymap_lookup(KeyMap *keymap, Key key)
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
