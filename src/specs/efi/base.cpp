#include "base.h"

namespace Efi {

Handle _handle = nullptr;
SystemTable* _st = nullptr;
Efi::LoadedImageProtocol* _li = nullptr;

Handle imageHandle() { return _handle; }

SystemTable* st() { return _st; }

BootService* bs() {
    return st()->boot;
}

RuntimeService* rt() {
    return st()->runtime;
}

Efi::LoadedImageProtocol* li() {
    if (not _li) {
        _li = Efi::openProtocol<Efi::LoadedImageProtocol>().unwrap();
    }

    return _li;
}

void init(Handle handle, SystemTable* st) {
    _handle = handle;
    _st = st;
}

} // namespace Efi
