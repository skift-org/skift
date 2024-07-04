#include <handover/hook.h>
#include <hjert-api/api.h>

#include <karm-ui/_embed.h>

namespace Karm::Ui::_Embed {

struct SkiftHost :
    public Host {
    Gfx::MutPixels _front;
    Strong<Gfx::Surface> _back;

    SkiftHost(Child root, Gfx::MutPixels front, Strong<Gfx::Surface> back)
        : Host(root),
          _front(front),
          _back(back) {
    }

    Gfx::MutPixels mutPixels() override {
        return _back->mutPixels();
    }

    void flip(Slice<Math::Recti> dirty) override {
        for (auto d : dirty) {
            Gfx::blitUnsafe(_front.clip(d), _back->pixels().clip(d));
        }
    }

    Res<> wait(TimeStamp) override {
        return Ok();
    }

    void bubble(Sys::Event &e) override {
        Host::bubble(e);
    }
};

Res<Strong<Host>> makeHost(Child root) {
    auto &handover = useHandover();

    auto *fb = handover.findTag(Handover::Tag::FB);
    auto fbVmo = try$(Hj::Vmo::create(Hj::ROOT, fb->start, fb->size, Hj::VmoFlags::DMA));
    try$(fbVmo.label("framebuffer"));
    static auto fbRange = try$(Hj::map(fbVmo, Hj::MapFlags::READ | Hj::MapFlags::WRITE));

    logInfo("fb: {x}-{x} {}x{}, {} stride", fbRange.range().start, fbRange.range().end(), fb->fb.width, fb->fb.height, fb->fb.pitch);

    Gfx::MutPixels front = {
        fbRange.mutBytes().buf(),
        {fb->fb.width, fb->fb.height},
        fb->fb.pitch,
        Gfx::BGRA8888,
    };

    auto back = Gfx::Surface::alloc(
        front.size(), Gfx::BGRA8888
    );

    return Ok(makeStrong<SkiftHost>(std::move(root), front, back));
}

} // namespace Karm::Ui::_Embed
