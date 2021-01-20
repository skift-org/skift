#pragma once

#include <libsettings/storage/Bundle.h>

namespace settings
{

struct Domain
{
    HashMap<String, Bundle> bundles;
};

} // namespace settings
