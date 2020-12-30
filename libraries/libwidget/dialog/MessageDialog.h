#pragma once

#include <libwidget/dialog/Dialog.h>
#include <libwidget/widgets/Button.h>
#include <libwidget/widgets/Container.h>
#include <libwidget/widgets/Label.h>

class MessageDialog : public Dialog
{
private:
    RefPtr<Icon> _icon{};
    String _title = "";
    String _message = "";
    int _buttons = 0;

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
        MessageDialog dialog;

        dialog.icon(icon);
        dialog.title(title);
        dialog.message(message);
        dialog.buttons(buttons);

        return dialog.show();
    }

    void icon(RefPtr<Icon> icon) { _icon = icon; }

    void title(String title) { _title = title; }

    void message(String message) { _message = message; }

    void buttons(int buttons) { _buttons = buttons; }

    void render(Window *window) override
    {
        window->icon(_icon);
        window->title(_title);
        window->size(Vec2i(300, 200));

        window->root()->layout(VFLOW(0));
        window->root()->insets(Insetsi(8));

        auto message_label = new Label(window->root(), _message, Anchor::CENTER);
        message_label->flags(Widget::FILL);

        auto container = new Container(window->root());
        container->layout(HFLOW(4));

        (new Container(container))->flags(Widget::FILL);

        if (_buttons & DialogButton::YES)
        {
            auto button = new Button(container, BUTTON_OUTLINE, "Yes");
            button->on(Event::ACTION, [&](auto) {
                close(DialogResult::YES);
                window->hide();
            });
        }

        if (_buttons & DialogButton::NO)
        {
            auto button = new Button(container, BUTTON_OUTLINE, "No");
            button->on(Event::ACTION, [&](auto) {
                close(DialogResult::NO);
                window->hide();
            });
        }

        if (_buttons & DialogButton::OK)
        {
            auto button = new Button(container, BUTTON_OUTLINE, "Ok");
            button->on(Event::ACTION, [&](auto) {
                close(DialogResult::OK);
                window->hide();
            });
        }

        if (_buttons & DialogButton::CANCEL)
        {
            auto button = new Button(container, BUTTON_OUTLINE, "Cancel");
            button->on(Event::ACTION, [&](auto) {
                close(DialogResult::CANCEL);
                window->hide();
            });
        }

        window->on(Event::WINDOW_CLOSING, [&](auto) {
            close(DialogResult::CLOSE);
            window->hide();
        });
    }
};