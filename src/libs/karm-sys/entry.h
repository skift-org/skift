#pragma once

#ifdef __KARM_SYS_ENTRY_INCLUDED
#    error "karm-sys/entry.h included twice"
#endif

#define __KARM_SYS_ENTRY_INCLUDED

#include "chan.h"
#include "context.h"

#if defined(__ck_sys_linux__) || defined(__ck_sys_darwin__)
#    define EMBED_POSIX_MAIN_IMPL
#    include <impl-posix/entry.h>
#elif defined(__ck_sys_skift__)
// Nothing to do, skift uses entryPoint by default
#elif defined(__ck_sys_efi__)
#    include <impl-efi/entry.h>
#elif defined(__ck_sys_wasm__)
#    include <impl-wasm/entry.h>
#else
#    error "Unknown system"
#endif
