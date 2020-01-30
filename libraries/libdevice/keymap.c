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