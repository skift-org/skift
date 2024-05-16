#pragma once

#include <karm-base/std.h>

#define wasm_import(name) __attribute__((import_module("env"), import_name(#name))) name
#define wasm_export(name) __attribute__((visibility("default"), export_name(#name))) name

extern "C" u64 wasm_import(embedGetTimeStamp)();
extern "C" void wasm_import(embedConsoleLog)(u8 const *str, usize len);
extern "C" void wasm_import(embedConsoleError)(u8 const *str, usize len);
extern "C" void *wasm_import(embedAlloc)(usize size);