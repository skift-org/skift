#pragma once

#include "object.h"

namespace Hjert::Core {

struct IoRange : public Object<IoRange> {

    static Res<Strong<IoRange>> create();
};

} // namespace Hjert::Core
