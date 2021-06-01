#include <libio/Format.h>

#include <libwidget/Application.h>
#include <libwidget/Components.h>
#include <libwidget/Elements.h>
#include <libwidget/Screen.h>

namespace Onboarding
{

struct Page
{
    String header;
    Vector<String> lines;
    String footer;
};

static const Vector<Page> PAGES = {
    {
        "Welcome to skiftOS!",
        {
            "skiftOS is a simple, handmade operating system for the x86 platform,",
            "aiming for clean and pretty APIs while keeping the spirit of UNIX.",
        },
        "You can skip at any time and start using skiftOS.",
    },
    {
        "Getting Started!",
        {
            "Click on the top left of the screen to bring up the application menu.",
            "Try out the terminal with familiar unix utilities.",
        },
        "Explore, experiment and enjoy.",
    },
    {
        "Join The Community.",
        {
            "The community is full of great people, join us on:",
            "Github, Discord, and Twitter #skiftOS.",
        },
        "https://skiftos.org/community",
    },
    {
        "Contribute to the project",
        {
            "Do you like skiftOS and want it to become even better?",
            "Then there are many way to contribute to the project:",
            "contributing code, reporting issues, donations, and sponsorship",
        },
        "https://skiftos.org/contributing",
    },
    {
        "All Done!",
        {
            "Thanks for choosing skiftOS!",
        },
        "<3",
    },
};

auto page(auto state)
{
    Vector<RefPtr<Widget::Element>> elements;

    elements.push_back(Widget::spacing({0, 16, 0}, Widget::label(PAGES[state].header, Math::Anchor::CENTER)));

    for (auto &line : PAGES[state].lines)
    {
        elements.push_back(Widget::label(line, Math::Anchor::CENTER));
    }

    elements.push_back(Widget::spacing({16, 0, 0}, Widget::label(PAGES[state].footer, Math::Anchor::CENTER)));

    return Widget::spacing(16, Widget::vflow(4, elements));
}

auto illustration(auto state)
{
    return Widget::min_height(
        180,
        Widget::image(
            Graphic::Bitmap::load_from_or_placeholder(IO::format("/Applications/onboarding/illustration{}.png", state)),
            Graphic::BitmapScaling::COVER));
}

auto finish_button()
{
    return Widget::filled_button("Finish", [=] {
        Widget::Application::the().exit(PROCESS_SUCCESS);
    });
}

auto next_button(auto state, auto update)
{
    return Widget::filled_button("Next", [=] {
        update(state + 1);
    });
}

auto navigation(auto state, auto update)
{
    return Widget::spacing(
        16,
        Widget::hflow({
            Widget::enable_if(state > 0, Widget::basic_button("Previous", [=] { update(state - 1); })),
            Widget::fill(Widget::stack(Widget::dots(PAGES.count(), state))),
            (state + 1 < (int)PAGES.count() ? next_button(state, update) : finish_button()),
        }));
}

struct Window : public Widget::Window
{
    RefPtr<Widget::Element> build() override
    {
        // clang-format off

        return Widget::min_size({550, 500},
            Widget::vflow({
                Widget::titlebar(Graphic::Icon::get("home"), "Welcome"),
                Widget::fill(
                    Widget::stateful(0, [](auto state, auto update) {
                        return Widget::vflow(6, {
                            illustration(state),
                            page(state),
                            Widget::spacer(),
                            navigation(state, update),
                        });
                    })
                ),
            })
        );

        // clang-format one
    }
};

struct Application : public Widget::Application
{
    OwnPtr<Widget::Window> build() override
    {
        return own<Window>();
    }
};

} // namespace Onboarding

int main(int argc, char const *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    Onboarding::Application app;
    return app.run();
}
