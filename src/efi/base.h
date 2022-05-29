#pragma once

#include <efi/spec.h>

namespace Efi {

Handle imageHandle();

SystemTable *st();

void init(Handle handle, SystemTable *st);

} // namespace Efi
