#pragma once

#define wasm_import(name) __attribute__((import_module("env"), import_name(#name))) name
#define wasm_export(name) __attribute__((visibility("default"), export_name(#name))) name
