#pragma once

#include <karm-base/rc.h>
#include <karm-base/res.h>

namespace Karm::Gpu {

struct Device {
    static Res<Strong<Device>> open();

    virtual ~Device() = default;
};

struct Shader {
    virtual ~Shader() = default;
};

struct CommandBuffer {
    virtual ~CommandBuffer() = default;
};

struct RenderPass {
    virtual ~RenderPass() = default;
};

struct Pipeline {
    virtual ~Pipeline() = default;
};

struct Buffer {
    virtual ~Buffer() = default;
};

struct Texture {
    virtual ~Texture() = default;
};

} // namespace Karm::Gpu
