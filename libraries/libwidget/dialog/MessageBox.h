#pragma once

#include <libwidget/dialog/Dialog.h>
#include <libwidget/widgets/Button.h>
#include <libwidget/widgets/Container.h>
#include <libwidget/widgets/Label.h>

class MessageBox : public Dialog
{
private:
    RefPtr<Icon> _icon{};
    String _message = "";

public:
    static DialogResult create_and_show(String title, String message)
    {
        return create_and_show(title, message, Icon::get("info"), DialogButton::OK);
    }

    static DialogResult create_and_show(String title, String message, RefPtr<Icon> icon)
    {
        return create_and_show(title, message, icon, DialogButton::OK);
    }

    static DialogResult create_and_show(String title, String message, RefPtr<Icon> icon, int buttons)
    {
        MessageBox dialog;

        dialog.icon(icon);
        dialog.title(title);
        dialog.message(message);
        dialog.buttons(buttons);

        return dialog.show();
    }

    void icon(RefPtr<Icon> icon) { _icon = icon; }

    void message(String message) { _message = message; }

    void render(Window *window) override
    {
        window->icon(_icon);

        window->size(Vec2i(300, 200));

        window->root()->layout(VFLOW(0));
        window->root()->insets(Insetsi(8));

        auto message_label = new Label(window->root(), _message, Anchor::CENTER);
        message_label->flags(Widget::FILL);

        auto container = new Container(window->root());
        container->layout(HFLOW(4));

        (new Container(container))->flags(Widget::FILL);

        create_buttons(container);
    }
};