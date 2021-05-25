#pragma once

#include <libgraphic/Icon.h>
#include <libwidget/Application.h>
#include <libwidget/Elements.h>

namespace Widget
{

enum DialogButton
{
    YES = (1 << 0),
    NO = (1 << 1),
    OK = (1 << 2),
    CANCEL = (1 << 3)
};

enum struct DialogResult
{
    NONE,
    YES,
    NO,
    OK,
    CANCEL,
    CLOSE,
};

struct Dialog
{
private:
    DialogResult _result = DialogResult::NONE;

    int _buttons = 0;
    String _title = "";
    Window *_window = nullptr;

public:
    String title() { return _title; }

    void title(String title) { _title = title; }

    void buttons(int buttons) { _buttons = buttons; }

    Dialog()
    {
    }

    virtual ~Dialog()
    {
    }

    DialogResult show()
    {
        Assert::truth(_window == nullptr);

        _window = new Window(WINDOW_NONE);

        _window->type(WINDOW_TYPE_POPOVER);

        _window->on(Event::WINDOW_CLOSING, [&](auto) {
            close(DialogResult::CLOSE);
            _window->hide();
        });

        render(_window);

        _window->show();

        Application::the().run_nested();

        delete _window;
        _window = nullptr;

        return _result;
    }

    void close(DialogResult result)
    {
        Application::the().exit_nested(0);
        _result = result;
    }

    virtual void render(Window *) {}

    virtual void on_button(DialogButton) {}

    void create_buttons(RefPtr<Element> parent)
    {
        if (_buttons & DialogButton::YES)
        {
            parent->add(Widget::outline_button("Yes", [&] {
                on_button(DialogButton::YES);
                close(DialogResult::YES);
                _window->hide();
            }));
        }

        if (_buttons & DialogButton::NO)
        {
            parent->add(Widget::outline_button("No", [&] {
                on_button(DialogButton::NO);
                close(DialogResult::NO);
                _window->hide();
            }));
        }

        if (_buttons & DialogButton::OK)
        {
            parent->add(Widget::outline_button("Ok", [&] {
                on_button(DialogButton::OK);
                close(DialogResult::OK);
                _window->hide();
            }));
        }

        if (_buttons & DialogButton::CANCEL)
        {
            parent->add(Widget::outline_button("Cancel", [&] {
                on_button(DialogButton::CANCEL);
                close(DialogResult::CANCEL);
                _window->hide();
            }));
        }
    }
};

} // namespace Widget
