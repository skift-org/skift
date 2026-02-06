module;

#include <hal/io.h>

export module Hjert.Core:cpu;

import Karm.Core;
import Hjert.Api;
import :arch;

namespace Hjert::Core {

export struct Cpu {
    bool _retainEnabled = false;
    isize _depth = 0;

    void beginInterrupt() {
        _retainEnabled = false;
    }

    void endInterrupt() {
        _retainEnabled = true;
    }

    void retainDisable() {
        _retainEnabled = false;
    }

    void retainEnable() {
        _retainEnabled = true;
    }

    void retainInterrupts() {
        if (_retainEnabled) {
            disableInterrupts();
            _depth++;
        }
    }

    void releaseInterrupts() {
        if (_retainEnabled) {
            _depth--;
            if (_depth == 0) {
                enableInterrupts();
            }
        }
    }

    virtual void enableInterrupts() = 0;

    virtual void disableInterrupts() = 0;

    virtual void relaxe() = 0;
};

} // namespace Hjert::Core
