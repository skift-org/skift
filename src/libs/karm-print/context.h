#pragma once

namespace Karm::Print {

struct Context {
    virtual ~Context() = default;

    void pageBreak() {}
};

} // namespace Karm::Print
