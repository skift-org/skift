#pragma once

#include <karm-base/array.h>
#include <karm-base/rc.h>

namespace Hjert::Core {

struct CCap {
    virtual ~CCap() = default;
};

using CSlot = OptStrong<CCap>;

struct CNode : public CCap {
    Array<CSlot, 512> _slots;
};

} // namespace Hjert::Core
