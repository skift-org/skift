#pragma once

import Karm.Core;
import Karm.Sys;
import Abi.Ms;

#include <vaerk-efi/base.h>

void __panicHandler(Karm::PanicKind kind, char const* msg, usize len);

extern "C" Efi::Status efi_main(Efi::Handle handle, Efi::SystemTable* st) {
    Efi::init(handle, st);
    Abi::Ms::init();
    Karm::registerPanicHandler(__panicHandler);

    (void)Efi::st()
        ->conOut->clearScreen(Efi::st()->conOut);

    char const* self = "efi-app";
    char const* argv[] = {self, nullptr};
    char const* argvp[] = {nullptr};
    Sys::Env env{1, argv, argvp, Ref::Url::parse("file:")};
    Async::Cancellation cancellation;
    Res<> code = Async::run(entryPointAsync(env, cancellation.token()));
    if (not code) {
        Error error = code.none();
        (void)Io::format(Sys::err(), "{}: {}\n", self, error.msg());
        return 1;
    }

    return EFI_SUCCESS;
}
