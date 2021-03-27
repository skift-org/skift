#include <libwidget/Container.h>
#include <libwidget/Label.h>
#include <libwidget/Spacer.h>
#include <libwidget/dialog/MessageBox.h>

namespace Widget
{

DialogResult MessageBox::create_and_show(String title, String message)
{
    return create_and_show(title, message, Graphic::Icon::get("info"), DialogButton::OK);
}

DialogResult MessageBox::create_and_show(String title, String message, RefPtr<Graphic::Icon> icon)
{
    return create_and_show(title, message, icon, DialogButton::OK);
}

DialogResult MessageBox::create_and_show(String title, String message, RefPtr<Graphic::Icon> icon, int buttons)
{
    MessageBox dialog;

    dialog.icon(icon);
    dialog.title(title);
    dialog.message(message);
    dialog.buttons(buttons);

    return dialog.show();
}

void MessageBox::render(Window *window)
{
    window->icon(_icon);

    window->size(Vec2i(300, 200));

    window->root()->layout(VFLOW(0));
    window->root()->insets(Insetsi(8));

    auto message_label = new Label(window->root(), _message, Anchor::CENTER);
    message_label->flags(Component::FILL);

    auto container = new Container(window->root());
    container->layout(HFLOW(4));

    new Spacer(container);

    create_buttons(container);
}

} // namespace Widget
