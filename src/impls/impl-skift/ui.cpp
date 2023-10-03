#include <handover/hook.h>
#include <hjert-api/api.h>

#include <karm-ui/_embed.h>
namespace Karm::Ui::_Embed {

struct Host :
    public Ui::Host {
    Gfx::MutPixels _front;
    Media::Image _back;

    Host(Ui::Child root, Gfx::MutPixels front, Media::Image back)
        : Ui::Host(root),
          _front(front),
          _back(back) {
    }

    Gfx::MutPixels mutPixels() override {
        return _back;
    }

    void flip(Slice<Math::Recti> dirty) override {
        for (auto d : dirty) {
            _front.blit(d, _back.pixels().clip(d));
        }
    }

    void pump() override {
    }

    void wait(TimeSpan) override {
    }

    void bubble(Async::Event &e) override {
        Ui::Host::bubble(e);
    }
};

Res<Strong<Karm::Ui::Host>> makeHost(Ui::Child root) {
    auto &handover = useHandover();

    auto *fb = handover.findTag(Handover::Tag::FB);
    auto fbVmo = try$(Hj::Vmo::create(Hj::ROOT, fb->start, fb->size, Hj::VmoFlags::DMA));
    try$(fbVmo.label("framebuffer"));
    static auto fbRange = try$(Hj::map(fbVmo, Hj::MapFlags::READ | Hj::MapFlags::WRITE));

    logInfo("fb: {x}-{x} {}x{}, {} stride",
            fbRange.range().start,
            fbRange.range().end(),
            fb->fb.width,
            fb->fb.height,
            fb->fb.pitch);

    Gfx::MutPixels front = {
        fbRange.mutBytes().buf(),
        {fb->fb.width, fb->fb.height},
        fb->fb.pitch,
        Gfx::BGRA8888,
    };

    auto back = Media::Image::alloc(
        front.size(), Gfx::BGRA8888);

    return Ok(makeStrong<Host>(std::move(root), front, back));
}

} // namespace Karm::Ui::_Embed
