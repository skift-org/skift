#include <efi/base.h>
#include <karm-ui/drag.h>

#include <karm-ui/_embed.h>

namespace Karm::Ui::_Embed {

struct EfiHost :
    public Ui::Host {
    Efi::SimpleTextInputProtocol *_stip = nullptr;
    Gfx::MutPixels _front;
    Media::Image _back;

    EfiHost(
        Ui::Child root,
        Efi::SimpleTextInputProtocol *stip,
        Gfx::MutPixels front,
        Media::Image back)
        : Ui::Host(root),
          _stip(stip),
          _front(front),
          _back(back) {
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
        usize eventIdx = {};
        Efi::bs()->waitForEvent(1, &_stip->waitForKey, &eventIdx).unwrap();
        Efi::Key key = {};
        _stip->readKeyStroke(_stip, &key).unwrap();
        auto e = key.toKeyEvent();
        event<Events::KeyboardEvent>(*this, e);
    }

    void wait(TimeSpan) override {
    }

    void bubble(Async::Event &e) override {
        Ui::Host::bubble(e);
    }
};

Res<Strong<Karm::Ui::Host>> makeHost(Ui::Child root) {
    auto *stip = try$(Efi::locateProtocol<Efi::SimpleTextInputProtocol>());
    auto *gop = try$(Efi::locateProtocol<Efi::GraphicsOutputProtocol>());
    auto *mode = gop->mode;

    logInfo("efi: gop: {}x{}, {} stride, {} modes", mode->info->horizontalResolution, mode->info->verticalResolution, mode->info->pixelsPerScanLine * 4, mode->maxMode);
    Gfx::MutPixels front = {
        (void *)mode->frameBufferBase,
        {(u16)mode->info->horizontalResolution, (u16)mode->info->verticalResolution},
        (u16)(mode->info->pixelsPerScanLine * 4),
        Gfx::BGRA8888,
    };

    auto back = Media::Image::alloc({front.width(), front.height()}, Gfx::BGRA8888);

    return Ok(makeStrong<EfiHost>(root, stip, front, back));
}

} // namespace Karm::Ui::_Embed
