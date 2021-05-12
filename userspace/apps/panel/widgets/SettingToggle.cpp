#include "panel/widgets/SettingToggle.h"

using namespace Widget;

namespace Panel
{

SettingToggleComponent::SettingToggleComponent(RefPtr<Graphic::Icon> icon, String name, const char *setting)
    : _name(name),
      _icon(icon)
{
    _setting = own<Settings::Setting>(setting, [this](auto &value) {
        update(value.as_bool());
    });
}

RefPtr<Element> SettingToggleComponent::build(bool state)
{
    // clang-format off

    return basic_button(
        spacing({0, 0, 12, 16},
            hflow(8, {
                icon(_icon),
                fill(label(_name)),
                toggle(state),
            })
        ),
        [this, state] {
            _setting->write(!state);
        }
    );

    // clang-format on
}

} // namespace Panel
