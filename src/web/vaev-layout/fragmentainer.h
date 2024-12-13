#pragma once

#include <karm-image/picture.h>
#include <karm-text/run.h>
#include <vaev-markup/dom.h>
#include <vaev-style/computer.h>

#include "frag.h"

namespace Vaev::Layout {

// https://www.w3.org/TR/css-break-3/#fragmentainer
// https://www.w3.org/TR/css-break-3/#fragmentation-context
struct FragmentationContext {

    Vec2Px currSize = {Limits<Px>::MAX, Limits<Px>::MAX};

    FragmentationContext() {}

    FragmentationContext(Vec2Px currSize) : currSize(currSize) {}

    // discovery mode:
    // first pass that identifies the breakpoints (forced or not); that is, breakpoints are only created and manipulated
    // in discovery mode
    bool _isDiscoveryMode = false;

    void enterDiscovery() { _isDiscoveryMode = true; }

    void leaveDiscovery() { _isDiscoveryMode = false; }

    bool isDiscoveryMode() {
        return allowBreak() and _isDiscoveryMode;
    }

    // NOTE: a bit rudimentar, but necessary while some displays do not have fragmentation implemented
    usize monolithicCount = 0;

    void enterMonolithicBox() {
        monolithicCount++;
    }

    void leaveMonolithicBox() {
        monolithicCount--;
    }

    bool hasInfiniteDimensions() {
        return currSize == Vec2Px{Limits<Px>::MAX, Limits<Px>::MAX};
    }

    bool allowBreak() {
        return not hasInfiniteDimensions() and monolithicCount == 0;
    }

    bool acceptsFit(Px verticalPosition, Px verticalSize, Px pendingVerticalSizes) {
        return verticalPosition + verticalSize + pendingVerticalSizes <= currSize.y;
    }
};

} // namespace Vaev::Layout
