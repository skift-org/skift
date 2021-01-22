#pragma once

#include <libwidget/dialog/Dialog.h>

class MessageBox : public Dialog
{
private:
    RefPtr<Icon> _icon{};
    String _message = "";

public:
    static DialogResult create_and_show(String title, String message);

    static DialogResult create_and_show(String title, String message, RefPtr<Icon> icon);

    static DialogResult create_and_show(String title, String message, RefPtr<Icon> icon, int buttons);

    void icon(RefPtr<Icon> icon) { _icon = icon; }

    void message(String message) { _message = message; }

    void render(Window *window) override;
};