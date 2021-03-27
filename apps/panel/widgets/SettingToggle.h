#pragma once

#include <libsettings/Setting.h>
#include <libwidget/Button.h>
#include <libwidget/IconPanel.h>
#include <libwidget/Label.h>
#include <libwidget/Switch.h>

namespace panel
{

class SettingToggle : public Widget::Button
{
private:
    String _name;
    RefPtr<Graphic::Icon> _icon;
    OwnPtr<Settings::Setting> _setting;
    bool _enabled;

public:
    SettingToggle(Widget::Component *parent, String name, RefPtr<Graphic::Icon> icon, const char *setting)
        : Button(parent, Button::TEXT),
          _name(name),
          _icon(icon)
    {
        _setting = own<Settings::Setting>(setting, [this](auto &value) {
            _enabled = value.as_bool();
            render();
        });

        render();
    }

    ~SettingToggle()
    {
    }

    void render()
    {
        clear_children();

        insets({12, 12, 12, 12});

        auto icon = new Widget::IconPanel(this, _icon);
        icon->insets(Insetsi(0, 0, 0, 4));

        auto label = new Widget::Label(this, _name);
        label->flags(Component::FILL);

        auto sw = new Widget::Switch(this);
        sw->state(_enabled);
    }

    void event(Widget::Event *event) override
    {
        if (event->type == Widget::Event::ACTION)
        {
            _setting->write(!_enabled);
            event->accepted = true;
        }

        Button::event(event);
    }
};

} // namespace panel
