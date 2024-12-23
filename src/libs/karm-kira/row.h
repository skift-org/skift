#pragma once

#include <karm-ui/input.h>

#include "_prelude.h"

namespace Karm::Kira {

Ui::Child rowContent(Ui::Child child);

Ui::Child rowContent(Opt<Ui::Child> leading, String title, Opt<String> subtitle, Opt<Ui::Child> trailing);

Ui::Child titleRow(String t);

Ui::Child pressableRow(Ui::OnPress onPress, Opt<Ui::Child> leading, String title, Opt<String> subtitle, Opt<Ui::Child> trailing);

Ui::Child buttonRow(Ui::OnPress onPress, Mdi::Icon i, String title, String subtitle);

Ui::Child buttonRow(Ui::OnPress onPress, String title, String text);

Ui::Child toggleRow(bool value, Ui::OnChange<bool> onChange, String title);

Ui::Child checkboxRow(bool value, Ui::OnChange<bool> onChange, String title);

Ui::Child radioRow(bool value, Ui::OnChange<bool> onChange, String title);

Ui::Child sliderRow(f64 value, Ui::OnChange<f64> onChange, String title);

Ui::Child selectRow(Ui::Child value, Ui::Slots options, String title);

Ui::Child colorRow(Gfx::Color color, Ui::OnChange<Gfx::Color> onChange, String title);

Ui::Child numberRow(f64 value, Ui::OnChange<f64> onChange, f64 step, String title);

Ui::Child treeRow(Opt<Ui::Slot> leading, String title, Opt<String> subtitle, Ui::Slot child);

Ui::Child treeRow(Opt<Ui::Slot> leading, String title, Opt<String> subtitle, Ui::Slots children);

} // namespace Karm::Kira
