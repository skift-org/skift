#include <efi/base.h>
#include <embed-ui/ui.h>
#include <karm-ui/drag.h>

namespace Embed {

struct EfiHost :
    public Ui::Host {
    Gfx::MutPixels _pixels;

    EfiHost(Ui::Child root, Gfx::MutPixels pixels)
        : Ui::Host(root), _pixels(pixels) {
        _dirty.pushBack(pixels.bound());
    }

    Gfx::MutPixels mutPixels() override {
        return _pixels;
    }

    void flip(Slice<Math::Recti>) override {
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

    Gfx::MutPixels pixels = {
        (void *)mode->frameBufferBase,
        {(u16)mode->info->horizontalResolution, (u16)mode->info->verticalResolution},
        (u16)(mode->info->pixelsPerScanLine * 4),
        Gfx::BGRA8888,
    };

    return Ok(makeStrong<EfiHost>(root, pixels));
}

} // namespace Embed
