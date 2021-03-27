#include <libio/Format.h>

#include <libwidget/Application.h>
#include <libwidget/Button.h>
#include <libwidget/Container.h>
#include <libwidget/Label.h>
#include <libwidget/Panel.h>
#include <libwidget/TitleBar.h>

#include "calculator/Calculator.h"

int main(int argc, char **argv)
{
    using namespace Widget;

    Application::initialize(argc, argv);

    auto window = new Window(WINDOW_NONE);
    window->title("Calculator");
    window->icon(Graphic::Icon::get("calculator-variant"));
    window->root()->layout(VFLOW(0));

    Calculator calculator{};

    new TitleBar(window->root());

    // Calculator screen
    auto screen_container = new Panel(window->root());
    auto screen_label = new Label(screen_container, "#value", Anchor::CENTER);
    screen_label->flags(Component::FILL);
    screen_label->outsets(16);
    auto calculator_observer = calculator.observe([&](Calculator &calculator) {
        screen_label->text(IO::format("{}", calculator.screen()));
    });

    // Calculator Buttons
    auto buttons_container = new Container(window->root());
    buttons_container->layout(GRID(4, 6, 4, 4));
    buttons_container->insets(6);

    auto button_percent = new Button(buttons_container, Button::OUTLINE, Graphic::Icon::get("percent"));
    __unused(button_percent);

    auto button_clear_all = new Button(buttons_container, Button::OUTLINE, "CE");
    button_clear_all->on(Event::ACTION, [&](auto) { calculator.clear_all(); });

    auto button_clear = new Button(buttons_container, Button::OUTLINE, "C");
    button_clear->on(Event::ACTION, [&](auto) { calculator.clear(); });

    auto button_backspace = new Button(buttons_container, Button::OUTLINE, Graphic::Icon::get("backspace-outline"));
    __unused(button_backspace);

    auto button_reciprocal = new Button(buttons_container, Button::OUTLINE, "1/x");
    button_reciprocal->on(Event::ACTION, [&](auto) { calculator.enter_operation(Operation::RECIPROCAL); });

    auto button_exp = new Button(buttons_container, Button::OUTLINE, Graphic::Icon::get("exponent"));
    button_exp->on(Event::ACTION, [&](auto) { calculator.enter_operation(Operation::POWER); });

    auto button_sqrt = new Button(buttons_container, Button::OUTLINE, Graphic::Icon::get("square-root"));
    button_sqrt->on(Event::ACTION, [&](auto) { calculator.enter_operation(Operation::SQRT); });

    auto button_divide = new Button(buttons_container, Button::OUTLINE, Graphic::Icon::get("slash-forward"));
    button_divide->on(Event::ACTION, [&](auto) { calculator.enter_operation(Operation::DIVIDE); });

    auto button_7 = new Button(buttons_container, Button::OUTLINE, "7");
    button_7->on(Event::ACTION, [&](auto) { calculator.enter_number(7); });

    auto button_8 = new Button(buttons_container, Button::OUTLINE, "8");
    button_8->on(Event::ACTION, [&](auto) { calculator.enter_number(8); });

    auto button_9 = new Button(buttons_container, Button::OUTLINE, "9");
    button_9->on(Event::ACTION, [&](auto) { calculator.enter_number(9); });

    auto button_multiply = new Button(buttons_container, Button::OUTLINE, Graphic::Icon::get("close"));
    button_multiply->on(Event::ACTION, [&](auto) { calculator.enter_operation(Operation::MULTIPLY); });

    auto button_4 = new Button(buttons_container, Button::OUTLINE, "4");
    button_4->on(Event::ACTION, [&](auto) { calculator.enter_number(4); });

    auto button_5 = new Button(buttons_container, Button::OUTLINE, "5");
    button_5->on(Event::ACTION, [&](auto) { calculator.enter_number(5); });

    auto button_6 = new Button(buttons_container, Button::OUTLINE, "6");
    button_6->on(Event::ACTION, [&](auto) { calculator.enter_number(6); });

    auto button_minus = new Button(buttons_container, Button::OUTLINE, Graphic::Icon::get("minus"));
    button_minus->on(Event::ACTION, [&](auto) { calculator.enter_operation(Operation::SUBSTRACT); });

    auto button_1 = new Button(buttons_container, Button::OUTLINE, "1");
    button_1->on(Event::ACTION, [&](auto) { calculator.enter_number(1); });

    auto button_2 = new Button(buttons_container, Button::OUTLINE, "2");
    button_2->on(Event::ACTION, [&](auto) { calculator.enter_number(2); });

    auto button_3 = new Button(buttons_container, Button::OUTLINE, "3");
    button_3->on(Event::ACTION, [&](auto) { calculator.enter_number(3); });

    auto button_plus = new Button(buttons_container, Button::OUTLINE, Graphic::Icon::get("plus"));
    button_plus->on(Event::ACTION, [&](auto) { calculator.enter_operation(Operation::ADD); });

    auto button_plus_minus = new Button(buttons_container, Button::OUTLINE, Graphic::Icon::get("plus-minus-variant"));
    __unused(button_plus_minus);

    auto button_0 = new Button(buttons_container, Button::OUTLINE, "0");
    button_0->on(Event::ACTION, [&](auto) { calculator.enter_number(0); });

    auto button_dot = new Button(buttons_container, Button::OUTLINE, ".");
    __unused(button_dot);

    auto button_equal = new Button(buttons_container, Button::FILLED, Graphic::Icon::get("equal"));
    button_equal->on(Event::ACTION, [&](auto) { calculator.equal(); });

    calculator.did_update();

    window->resize_to_content();
    window->show();

    return Application::run();
}
