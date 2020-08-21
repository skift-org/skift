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
    auto cat = Bitmap::load_from_or_placeholder("/Applications/splash-screen/cat.png");

    auto logo_on_screen_bound = logo->bound().centered_within(framebuffer->resolution());

    framebuffer->painter().clear(COLOR_RGBA(0x33, 0x33, 0x33, 255));

    framebuffer->painter().blit_bitmap(*logo, logo->bound(), logo_on_screen_bound);

    auto loading_bar_on_screen_bound = Rectangle(0, 0, logo_on_screen_bound.width() * 1.4, 4)
                                           .centered_within(framebuffer->resolution())
                                           .offset(Vec2i(0, logo_on_screen_bound.height() + 32));

    framebuffer->mark_dirty_all();
    framebuffer->blit();

    for (size_t i = 0; i <= 100; i++)
    {
        framebuffer->painter().clear_rectangle(loading_bar_on_screen_bound.expended(Insets(16, 16)), COLOR_RGBA(0x33, 0x33, 0x33, 255));
        framebuffer->painter().clear_rectangle(loading_bar_on_screen_bound, COLOR_RGBA(0x44, 0x44, 0x44, 255));

        Rectangle progress = loading_bar_on_screen_bound.take_left(loading_bar_on_screen_bound.width() * (i / 100.0));

        if (argc == 2 && strcmp(argv[1], "--nyan") == 0)
        {
            framebuffer->painter().clear_rectangle(progress, HSV((int)(360 * (i / 100.0) * 2) % 360, 0.5, 1));
            framebuffer->painter().blit_bitmap(*cat, cat->bound(), cat->bound().moved(progress.top_right() + Vec2i(-4, -2 - 8)));
        }
        else
        {
            framebuffer->painter().clear_rectangle(progress, COLOR_RGBA(0x00, 0x66, 0xff, 255));
            framebuffer->painter().fill_rectangle(progress.take_right(1), COLOR_RGBA(0xff, 0xff, 0xff, 100));
        }

        framebuffer->mark_dirty(loading_bar_on_screen_bound.expended(Insets(16)));
        framebuffer->blit();

        process_sleep(5);
    }

    return 0;
}
