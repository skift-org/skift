
#include <libsystem/core/Plugs.h>

Tick system_get_ticks()
{
    return __plug_system_get_ticks();
}
