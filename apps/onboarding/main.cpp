#include <abi/Syscalls.h>

#include <libwidget/Application.h>
#include <libwidget/Screen.h>
#include <libwidget/Widgets.h>

int main(int argc, char **argv)
{
    Application::initialize(argc, argv);

    Window *window = new Window(WINDOW_BORDERLESS | WINDOW_ALWAYS_FOCUSED | WINDOW_TRANSPARENT);

    window->title("Onboarding");
    window->type(WINDOW_TYPE_PANEL);
    window->bound(Screen::bound());
    window->opacity(0);
    window->show();
    window->root()->layout(STACK());

    auto background = new Panel(window->root());

    background->layout(STACK());
    background->color(THEME_MIDDLEGROUND, Colors::BLACK.with_alpha(0.5));

    auto dialog = new Panel(background);

    dialog->max_width(400);
    dialog->max_height(350);
    dialog->layout(VFLOW(0));
    dialog->border_radius(6);

    auto illustration = new Panel(dialog);

    illustration->max_height(160);
    illustration->min_height(160);
    illustration->border_radius(6);
    illustration->color(THEME_MIDDLEGROUND, Colors::WHITE);

    new Image(illustration, Bitmap::load_from("/Applications/onboarding/illustration1.png").value());

    auto content = new Container(dialog);
    content->flags(Widget::FILL);

    auto navigation = new Container(dialog);

    navigation->layout(HFLOW(0));
    navigation->insets(8);

    auto skipall_button = new Button(navigation, BUTTON_TEXT, "Skip all");
    skipall_button->on(Event::ACTION, [](auto) {
        Application::exit(PROCESS_SUCCESS);
    });

    new Spacer(navigation);

    new Button(navigation, BUTTON_FILLED, "Next");

    return Application::run();
}
