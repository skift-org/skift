#include <karm-base/align.h>
#include <karm-base/string.h>
#include <karm-io/traits.h>

#include "externs.h"

void __panicHandler(Karm::PanicKind kind, char const *msg) {
    embedConsoleError(kind == Karm::PanicKind::PANIC ? (u8 const *)"PANIC: " : (u8 const *)"DEBUG: ", 7);
    embedConsoleError((u8 const *)msg, cstrLen(msg));
}