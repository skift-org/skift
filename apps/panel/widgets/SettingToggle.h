#pragma once

#include <libsettings/Setting.h>
#include <libwidget/Button.h>
#include <libwidget/IconPanel.h>
#include <libwidget/Label.h>
#include <libwidget/Switch.h>

namespace panel
{

class SettingToggle : public Button
{
private:
    String _name;
    RefPtr<Icon> _icon;
    OwnPtr<Settings::Setting> _setting;
    bool _enabled;

public:
    SettingToggle(Widget *parent, String name, RefPtr<Icon> icon, const char *setting)
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

        auto icon = new IconPanel(this, _icon);
        icon->insets(Insetsi(0, 0, 0, 4));

        auto label = new Label(this, _name);
        label->flags(Widget::FILL);

        auto sw = new Switch(this);
        sw->state(_enabled);
    }

    void event(Event *event) override
    {
        if (event->type == Event::ACTION)
        {
            _setting->write(!_enabled);
            event->accepted = true;
        }

        Button::event(event);
    }
};

} // namespace panel
