#pragma once

#ifdef __KARM_MAIN_INCLUDED
#    error "karm-main/main.h included twice"
#endif

#define __KARM_MAIN_INCLUDED

#if defined(__ck_sys_linux__) || defined(__ck_sys_darwin__)
#    define EMBED_POSIX_MAIN_IMPL
#    include <impl-posix/main.h>
#elif defined(__ck_sys_skift__)
// Nothing to do, skift uses the entryPoint directly.
#    include "base.h"

// Some code expect the karm-sys/chan.h to be included.
#    include <karm-sys/chan.h>
#elif defined(__ck_sys_efi__)
#    include <impl-efi/main.h>
#else
#    error "Unknown system"
#endif
