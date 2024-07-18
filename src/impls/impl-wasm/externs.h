#pragma once

#include <karm-base/size.h>

static constexpr usize PAGE_SIZE = kib(64);

#define wasm_import(name) __attribute__((import_module("env"), import_name(#name))) name
#define wasm_export(name) __attribute__((visibility("default"), export_name(#name))) name

extern "C" ExternSym __heap_base;
extern "C" ExternSym __heap_end;

extern "C" u64 wasm_import(embedGetTimeStamp)();
extern "C" void wasm_import(embedConsoleLog)(u8 const *str, usize len);
extern "C" void wasm_import(embedConsoleError)(u8 const *str, usize len);
extern "C" void *wasm_import(embedAlloc)(usize size);
