#pragma once

#include <libutils/Callback.h>
#include <libwidget/Panel.h>
#include <libwidget/model/TextModel.h>

namespace panel
{

class SearchBar : public Panel
{
public:
    SearchBar(Widget *parent, RefPtr<TextModel> model);
};

} // namespace panel
