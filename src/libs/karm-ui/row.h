#pragma once

#include "input.h"
#include "layout.h"
#include "react.h"
#include "view.h"

namespace Karm::Ui {

Child row(Child child);

Child row(Opt<Child> leading, String title, Opt<String> subtitle, Opt<Child> trailing);

Child titleRow(String t);

Child pressableRow(OnPress onPress, Opt<Child> leading, String title, Opt<String> subtitle, Opt<Child> trailing);

Child buttonRow(OnPress onPress, Mdi::Icon i, String title, String subtitle);

Child buttonRow(OnPress onPress, String title, String text);

Child toggleRow(bool value, OnChange<bool> onChange, String title);

Child checkboxRow(bool value, OnChange<bool> onChange, String title);

Child radioRow(bool value, OnChange<bool> onChange, String title);

Child sliderRow(SliderStyle style, f64 value, OnChange<f64> onChange, String title);

Child sliderRow(f64 value, OnChange<f64> onChange, String title);

Child colorRow(Gfx::Color color, OnChange<Gfx::Color> onChange, String title);

Child navRow(bool selected, OnPress onPress, Mdi::Icon i, String title);

Child navList(Children children);

Child navTree(Mdi::Icon icon, String title, Child child);

Child treeRow(Opt<Child> leading, String title, Opt<String> subtitle, Child child);

Child treeRow(Opt<Child> leading, String title, Opt<String> subtitle, Children children);

} // namespace Karm::Ui
