#include "task-manager/widgets/ProcessTable.h"

using namespace Widget;

namespace TaskManager
{

ProcessTableComponent::ProcessTableComponent(Ref<Widget::TableModel> model)
    : _model{model}
{
}

Ref<Element> ProcessTableComponent::build()
{
    return table(_model);
}

} // namespace TaskManager
