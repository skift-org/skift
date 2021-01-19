#include <libgraphic/Framebuffer.h>
#include <libsystem/process/Process.h>

static const auto BACKGROUND = Color::from_hex(0x18181B);
static const auto PROGRESS = Color::from_hex(0x0066ff);
static const auto REMAINING = Color::from_hex(0x444444);

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

    auto logo_container = logo->bound().centered_within(framebuffer->resolution());

    auto loading_container = Recti(0, 0, logo_container.width() * 1.4, 4)
                                 .centered_within(framebuffer->resolution())
                                 .offset(Vec2i(0, logo_container.height() + 26));

    auto &painter = framebuffer->painter();

    painter.clear(BACKGROUND);

    painter.blit(*logo, logo->bound(), logo_container);

    framebuffer->mark_dirty_all();
    framebuffer->blit();

    for (size_t i = 0; i <= 100; i++)
    {
        painter.clear(loading_container, REMAINING);

        Recti progress = loading_container.take_left(loading_container.width() * (i / 100.0));

        if (argc == 2 && strcmp(argv[1], "--nyan") == 0)
        {
            auto color = Color::from_hsv((int)(360 * (i / 100.0) * 2) % 360, 0.5, 1);

            painter.clear(progress, color);
            painter.blit(*cat, cat->bound(), cat->bound().moved(progress.top_right() + Vec2i(-4, -2 - 8)));
        }
        else
        {
            painter.clear(progress, PROGRESS);
            painter.fill_rectangle(progress.take_right(1), REMAINING);
        }

        framebuffer->mark_dirty(loading_container.expended(Insetsi(16)));
        framebuffer->blit();

        process_sleep(5);
    }

    return 0;
}
