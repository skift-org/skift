#include <stdio.h>

#include <libio/Format.h>
#include <libwidget/Application.h>
#include <libwidget/Button.h>
#include <libwidget/Container.h>
#include <libwidget/Image.h>
#include <libwidget/Label.h>
#include <libwidget/PaginationDots.h>
#include <libwidget/Panel.h>
#include <libwidget/Screen.h>
#include <libwidget/Spacer.h>

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

void pages(Widget *host, const Page &page)
{
    new Label(host, page.header, Anchor::CENTER);

    auto content = new Container(host);
    content->insets({8, 0});
    content->layout(VFLOW(2));

    for (auto &lines : page.lines)
    {
        new Label(content, lines, Anchor::CENTER);
    }

    new Label(host, page.footer, Anchor::CENTER);
}

int main(int argc, char **argv)
{
    Application::initialize(argc, argv);

    Window *window = new Window(WINDOW_BORDERLESS | WINDOW_ALWAYS_FOCUSED | WINDOW_ACRYLIC | WINDOW_NO_ROUNDED_CORNERS);

    window->title("Onboarding");
    window->type(WINDOW_TYPE_POPOVER);
    window->bound(Screen::bound());
    window->opacity(0);
    window->show();
    window->root()->layout(STACK());

    auto background = new Panel(window->root());

    background->layout(STACK());
    background->color(THEME_MIDDLEGROUND, Colors::BLACK.with_alpha(0.5));
    background->flags(Widget::FILL);

    auto dialog = new Panel(background);

    dialog->pin_width(420);
    dialog->pin_height(420);

    dialog->layout(VFLOW(0));
    dialog->border_radius(6);

    auto illustration = new Panel(dialog);
    illustration->min_height(160);
    illustration->border_radius(6);
    illustration->color(THEME_MIDDLEGROUND, Colors::WHITE);

    auto image = new Image(illustration, Bitmap::placeholder());
    image->flags(Widget::FILL);
    image->scaling(BitmapScaling::CENTER);

    auto content = new Container(dialog);
    content->flags(Widget::FILL);
    content->insets(16);
    content->layout(VFLOW(4));

    auto dots_container = new Container(dialog);
    dots_container->insets(16);

    auto dots = new PaginationDots(dots_container, 5);

    auto navigation = new Container(dialog);

    navigation->layout(HFLOW(4));
    navigation->insets(8);

    auto skipall_button = new Button(navigation, Button::TEXT, "Skip All");

    new Spacer(navigation);

    auto back_button = new Button(navigation, Button::OUTLINE, "Previous");

    auto next_button = new Button(navigation, Button::FILLED, "Next");

    int current_page = 0;

    auto set_current_page = [&](int index) {
        if (index == 5)
        {
            Application::exit(PROCESS_SUCCESS);
        }

        if (index < 0 || index > 4)
        {
            return;
        }

        current_page = index;

        skipall_button->enable_if(current_page < 4);
        back_button->enable_if(current_page > 0);
        dots->index(index);

        auto image_path = IO::format("/Applications/onboarding/illustration{}.png", index);
        image->change_bitmap(Bitmap::load_from(image_path).value());

        content->clear_children();
        pages(content, PAGES[index]);
    };

    set_current_page(0);

    skipall_button->on(Event::ACTION, [](auto) {
        Application::exit(PROCESS_SUCCESS);
    });

    back_button->on(Event::ACTION, [&](auto) {
        set_current_page(current_page - 1);
    });

    next_button->on(Event::ACTION, [&](auto) {
        set_current_page(current_page + 1);
    });

    window->on(Event::KEYBOARD_KEY_PRESS, [&](Event *event) {
        if (event->keyboard.key == KEYBOARD_KEY_ESC)
        {
            Application::exit(PROCESS_SUCCESS);
        }
        else if (event->keyboard.key == KEYBOARD_KEY_RIGHT)
        {
            set_current_page(current_page + 1);
        }
        else if (event->keyboard.key == KEYBOARD_KEY_LEFT)
        {
            set_current_page(current_page - 1);
        }
    });

    return Application::run();
}
