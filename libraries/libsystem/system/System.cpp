
#include <libsystem/core/Plugs.h>

uint system_get_ticks()
{
    return __plug_system_get_ticks();
}
