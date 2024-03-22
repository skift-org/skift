#pragma once

namespace Web {

enum struct Namespace {
#define NAMESPACE(IDENT, _) IDENT,
#include "defs/namespaces.inc"
#undef NAMESPACE
};

} // namespace Web
