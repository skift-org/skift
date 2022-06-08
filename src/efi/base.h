#pragma once

#include <efi/spec.h>

namespace Efi {

Handle imageHandle();

SystemTable *st();

BootService *bs();

RuntimeService *rt();

void init(Handle handle, SystemTable *st);

template <typename P>
static inline Result<P *> openProtocol(Handle handle) {
    P *result = nullptr;
    Uuid uuid = P::UUID;
    try$(bs()->openProtocol(handle, &uuid, (void **)&result, imageHandle(), nullptr, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL));
    return result;
}

template <typename P>
static inline Result<P *> openProtocol() {
    return openProtocol<P>(imageHandle());
}

template <typename P>
static inline Result<P> locateProtocol() {
    P *result = nullptr;
    Uuid uuid = P::UUID;
    try$(bs()->locateProtocol(&uuid, nullptr, &result));
    return result;
}

} // namespace Efi
