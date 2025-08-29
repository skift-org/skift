#pragma once

#include <efi/spec.h>
#include <karm-core/macros.h>

namespace Efi {

Handle imageHandle();

SystemTable* st();

BootService* bs();

RuntimeService* rt();

Efi::LoadedImageProtocol* li();

void init(Handle handle, SystemTable* st);

template <typename P>
inline Res<P*> openProtocol(Handle handle) {
    P* result = nullptr;
    Guid guid = P::GUID;
    try$(bs()->openProtocol(handle, &guid, (void**)&result, imageHandle(), nullptr, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL));
    return Ok(result);
}

template <typename P>
inline Res<P*> openProtocol() {
    return openProtocol<P>(imageHandle());
}

template <typename P>
inline Res<P*> locateProtocol() {
    P* result = nullptr;
    Guid guid = P::GUID;
    try$(bs()->locateProtocol(&guid, nullptr, (void**)&result));
    return Ok(result);
}

} // namespace Efi
