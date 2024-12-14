#pragma once

#include <karm-base/size.h>

#include "macros.h"

static constexpr usize PAGE_SIZE = kib(64);

extern "C" ExternSym __heap_base;
extern "C" ExternSym __heap_end;

extern "C" u64 wasm_import(embedGetTimeStamp)();
extern "C" void wasm_import(embedConsoleLog)(u8 const *str, usize len);
extern "C" void wasm_import(embedConsoleError)(u8 const *str, usize len);
extern "C" void *wasm_import(embedAlloc)(usize size);
