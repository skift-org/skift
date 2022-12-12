#pragma once

#ifdef __KARM_MAIN_INCLUDED
#    error "karm-main/main.h included twice"
#endif

#define __KARM_MAIN_INCLUDED

#if defined(__osdk_sys_linux__) or  \
    defined(__osdk_sys_darwin__) or \
    defined(__osdk_sys_brutal__)
#    define EMBED_POSIX_MAIN_IMPL
#    include <embed-posix/main.h>
#elif defined(__osdk_sys_efi__)
#    define EMBED_EFI_MAIN_IMPL
#    include <embed-efi/main.h>
#else
#    error "Unknown system"
#endif
