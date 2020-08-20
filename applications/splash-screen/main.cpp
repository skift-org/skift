#include <libgraphic/Framebuffer.h>
#include <libsystem/process/Process.h>

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    auto framebuffer_or_result = Framebuffer::open();

    if (!framebuffer_or_result.success())
    {
        return -1;
    }

    auto framebuffer = framebuffer_or_result.take_value();

    auto logo = Bitmap::load_from_or_placeholder("/Applications/splash-screen/logo.png");

    framebuffer->painter().clear(COLOR_RGBA(0x33, 0x33, 0x33, 255));

    framebuffer->painter().blit_bitmap(*logo, logo->bound(), logo->bound().centered_within(framebuffer->resolution()));

    framebuffer->mark_dirty_all();

    framebuffer->blit();

    process_sleep(500);

    return 0;
}
