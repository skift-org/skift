#include <abi/Syscalls.h>

#include <libwidget/Application.h>
#include <libwidget/Elements.h>
#include <libwidget/Layouts.h>
#include <libwidget/Screen.h>

namespace Logout
{

auto title()
{
    return Widget::hflow(4, {
                                Widget::square(Widget::icon(Graphic::Icon::get("power-standby"), Graphic::ICON_36PX)),
                                Widget::vflow(4, {
                                                     Widget::label("Shutdown or restart your computer.", Math::Anchor::LEFT),
                                                     Widget::label("Any unsaved work will be lost!", Math::Anchor::LEFT),
                                                 }),
                            });
}

auto shutdown_button()
{
    return Widget::basic_button(Graphic::Icon::get("power-standby"), "Shutdown", [&] {
        hj_system_shutdown();
    });
}

auto reboot_button()
{
    return Widget::basic_button(Graphic::Icon::get("restart"), "Reboot", [&] {
        hj_system_reboot();
    });
}

auto logoff_button()
{
    return Widget::basic_button(Graphic::Icon::get("logout"), "Logoff");
}

auto cancel_button(auto window)
{
    return Widget::filled_button("Cancel", [window] {
        window->hide();
    });
}

struct Window : public Widget::Window
{
    Window() : Widget::Window(WINDOW_FULLSCREEN |
                              WINDOW_BORDERLESS |
                              WINDOW_ALWAYS_FOCUSED |
                              WINDOW_ACRYLIC |
                              WINDOW_NO_ROUNDED_CORNERS)
    {
        type(WINDOW_TYPE_POPOVER);
        opacity(0.5);
    }

    RefPtr<Widget::Element> build() override
    {
        using namespace Widget;

        // clang-format off

        return stack(
            panel(6,
                spacing(6,
                    min_size({250, 250},
                            vflow(4, {
                                title(),

                                spacer(),

                                shutdown_button(),
                                reboot_button(),
                                logoff_button(),

                                spacer(),

                                cancel_button(this),
                            }
                        )
                    )
                )
            )
        );

        // clang-format on
    }
};

struct Application : public Widget::Application
{
    OwnPtr<Widget::Window> build() override
    {
        return own<Window>();
    }
};

} // namespace Logout

int main(int argc, char const *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    Logout::Application app;
    return app.run();
}
