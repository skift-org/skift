#include <efi/base.h>
#include <embed-ui/ui.h>
#include <karm-ui/drag.h>

namespace Embed {

struct EfiHost :
    public Ui::Host {
    Gfx::MutPixels _front;
    Media::Image _back;

    EfiHost(Ui::Child root, Gfx::MutPixels front, Media::Image back)
        : Ui::Host(root), _front(front), _back(back) {
        _dirty.pushBack(front.bound());
    }

    Gfx::MutPixels mutPixels() override {
        return _back;
    }

    void flip(Slice<Math::Recti> dirty) override {
        for (auto d : dirty)
            _front.blit(d, _back.pixels());
    }

    void pump() override {
        
    }

    void wait(usize) override {
    }

    void bubble(Events::Event &e) override {
        Ui::Host::bubble(e);
    }
};

Res<Strong<Karm::Ui::Host>> makeHost(Ui::Child root) {
    Efi::GraphicsOutputProtocol *gop = nullptr;
    Efi::bs()->locateProtocol(&Efi::GraphicsOutputProtocol::UUID, nullptr, (void **)&gop).unwrap();
    auto mode = gop->mode;

    logInfo("efi: gop: {}x{}, {} stride, {} modes", mode->info->horizontalResolution, mode->info->verticalResolution, mode->info->pixelsPerScanLine * 4, mode->maxMode);

    Gfx::MutPixels front = {
        (void *)mode->frameBufferBase,
        {(u16)mode->info->horizontalResolution, (u16)mode->info->verticalResolution},
        (u16)(mode->info->pixelsPerScanLine * 4),
        Gfx::BGRA8888,
    };

    auto back = Media::Image::alloc({front.width(), front.height()}, Gfx::BGRA8888);

    return Ok(makeStrong<EfiHost>(root, front, back));
}

} // namespace Embed
