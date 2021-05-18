#include "task-manager/widgets/ProcessTable.h"

using namespace Widget;

namespace TaskManager
{

ProcessTableComponent::ProcessTableComponent(RefPtr<Widget::TableModel> model)
    : _model{model}
{
}

RefPtr<Element> ProcessTableComponent::build()
{
    return table(_model);
}

} // namespace TaskManager
