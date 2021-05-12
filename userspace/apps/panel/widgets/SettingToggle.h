#pragma once

#include <libsettings/Setting.h>
#include <libwidget/Components.h>

namespace Panel
{

struct SettingToggleComponent :
    public Widget::StatefulComponent<bool>
{
private:
    String _name;
    RefPtr<Graphic::Icon> _icon;
    OwnPtr<Settings::Setting> _setting;

public:
    SettingToggleComponent(
        RefPtr<Graphic::Icon> icon,
        String name,
        const char *setting);

    RefPtr<Element> build(bool state) override;
};

WIDGET_BUILDER(SettingToggleComponent, setting_toggle);

} // namespace Panel