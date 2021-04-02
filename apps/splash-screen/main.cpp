#include <libgraphic/Framebuffer.h>
#include <libsystem/process/Process.h>

static const auto BACKGROUND = Graphic::Colors::BLACK;

int main(int, char *[])
{
    auto framebuffer_or_result = Graphic::Framebuffer::open();

    if (!framebuffer_or_result.success())
    {
        return -1;
    }

    auto framebuffer = framebuffer_or_result.unwrap();

    auto logo = Graphic::Bitmap::load_from_or_placeholder("/Applications/splash-screen/logo.png");

    auto logo_container = logo->bound().centered_within(framebuffer->resolution());

    auto &painter = framebuffer->painter();

    painter.clear(BACKGROUND);
    painter.blit(*logo, logo->bound(), logo_container);

    framebuffer->mark_dirty_all();
    framebuffer->blit();

    return 0;
}
