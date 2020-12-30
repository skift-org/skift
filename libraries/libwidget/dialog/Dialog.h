#pragma once

#include <libgraphic/Icon.h>

#include <libwidget/Application.h>

enum DialogButton
{
    YES = (1 << 0),
    NO = (1 << 1),
    OK = (1 << 2),
    CANCEL = (1 << 3)
};

enum class DialogResult
{
    NONE,
    YES,
    NO,
    OK,
    CANCEL,
    CLOSE,
};

class Dialog
{
private:
    DialogResult _result = DialogResult::NONE;

public:
    Dialog()
    {
    }

    DialogResult show()
    {
        Window *window = new Window(WINDOW_NONE);
        window->type(WINDOW_TYPE_DIALOG);

        render(window);

        window->show();

        Application::run_nested();

        delete window;

        return _result;
    }

    void close(DialogResult result)
    {
        Application::exit_nested(0);
        _result = result;
    }

    virtual void render(Window *) {}
};