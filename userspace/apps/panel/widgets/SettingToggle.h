#pragma once

#include <libsettings/Setting.h>
#include <libwidget/Components.h>

namespace panel
{

class SettingToggle : public Widget::Stateful<bool>
{
private:
    String _name;
    RefPtr<Graphic::Icon> _icon;
    OwnPtr<Settings::Setting> _setting;

public:
    SettingToggle(RefPtr<Graphic::Icon> icon, String name, const char *setting)
        : _name(name),
          _icon(icon)
    {
        _setting = own<Settings::Setting>(setting, [this](auto &value) {
            update(value.as_bool());
        });
    }

    virtual RefPtr<Element> build(bool state)
    {
        return Widget::basic_button(
            Widget::spacing({0, 0, 12, 16},
                            Widget::hflow(8, {
                                                 Widget::icon(_icon),
                                                 Widget::fill(Widget::label(_name)),
                                                 Widget::toggle(state),
                                             })),
            [this, state] {
                _setting->write(!state);
            });
    }
};

WIDGET_BUILDER(SettingToggle, setting_toggle);

} // namespace panel
