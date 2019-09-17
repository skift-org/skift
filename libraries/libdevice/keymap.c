#include <libdevice/keymap.h>

static keymap_keybing_t* keymap_lookup(keymap_t* this, key_t key)
{
    for (int i = 0; i < this->bindings_count; i++)
    {
        if( this->bindings[i].key == key)
        {
            return &this->bindings[i];
        }
    }
    
    return NULL;
}