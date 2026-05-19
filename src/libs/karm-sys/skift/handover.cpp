module;

#include <vaerk-handover/spec.h>

export module Karm.Sys.Skift:handover;

import Karm.Core;
import Karm.Ref;
import Karm.Sys;

namespace Karm::Sys::Skift {

export Handover::Payload* globalPayload = nullptr;

export Handover::Payload& useHandover() {
    return *globalPayload;
}

} // namespace Karm::Sys::Skift
