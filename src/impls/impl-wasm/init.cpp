#include <karm-base/base.h>

extern "C" void __wasm_call_ctors();

namespace Abi::Wasm {

__attribute__((export_name("_initialize"))) void init() {
    __wasm_call_ctors();
}

void fini() {
}

} // namespace Abi::Wasm
