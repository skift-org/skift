#include <embed-ui/ui.h>

namespace Embed {

struct EfiHost :
    public Ui::Host {

    EfiHost(Ui::Child root)
        : Ui::Host(root) {
    }

    Gfx::MutPixels mutPixels() override {
        panic("not implemented");
    }

    void flip(Slice<Math::Recti>) override {
        panic("not implemented");
    }

    void pump() override {
        panic("not implemented");
    }

    void wait(usize) override {
    }
};

Res<Strong<Karm::Ui::Host>> makeHost(Ui::Child) {
    panic("not implemented");
}

} // namespace Embed
