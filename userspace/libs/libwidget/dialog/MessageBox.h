#pragma once

#include <libwidget/dialog/Dialog.h>

namespace Widget
{

struct MessageBox : public Dialog
{
private:
    Ref<Graphic::Icon> _icon{};
    String _message = "";

public:
    static DialogResult create_and_show(String title, String message);

    static DialogResult create_and_show(String title, String message, Ref<Graphic::Icon> icon);

    static DialogResult create_and_show(String title, String message, Ref<Graphic::Icon> icon, int buttons);

    void icon(Ref<Graphic::Icon> icon) { _icon = icon; }

    void message(String message) { _message = message; }

    void render(Window *window) override;
};

} // namespace Widget
