#pragma once

#include <libwidget/dialog/Dialog.h>

namespace Widget
{

class MessageBox : public Dialog
{
private:
    RefPtr<Graphic::Icon> _icon{};
    String _message = "";

public:
    static DialogResult create_and_show(String title, String message);

    static DialogResult create_and_show(String title, String message, RefPtr<Graphic::Icon> icon);

    static DialogResult create_and_show(String title, String message, RefPtr<Graphic::Icon> icon, int buttons);

    void icon(RefPtr<Graphic::Icon> icon) { _icon = icon; }

    void message(String message) { _message = message; }

    void render(Window *window) override;
};

} // namespace Widget
