#include "base.h"

namespace Efi {

Handle _handle;
SystemTable *_st;

Handle imageHandle() { return _handle; }

SystemTable *st() { return _st; }

BootService *bs(){
    return st()->boot;
}

RuntimeService *rt(){
    return st()->runtime;
}

void init(Handle handle, SystemTable *st) {
    _handle = handle;
    _st = st;
}

} // namespace Efi
