import Karm.Core;
import Hjert.Core;

#include <vaerk-handover/entry.h>

HandoverRequests$(
    Handover::requestStack(),
    Handover::requestFb(),
    Handover::requestFiles()
);

void __panicHandler(PanicKind kind, char const* buf, usize len) {
    if (kind == PanicKind::PANIC) {
        (void)Io::format(Hjert::Arch::globalOut(), "PANIC: {}\n", Str{buf, len});
        Hjert::Arch::stop();
        __builtin_unreachable();
    }
    (void)Io::format(Hjert::Arch::globalOut(), "DEBUG: {}\n", Str{buf, len});
}

Res<> entryPoint(u64 magic, Handover::Payload& payload) {
    Karm::registerPanicHandler(__panicHandler);
    return Hjert::Core::init(magic, payload);
}
