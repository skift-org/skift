#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>

#include "mock.h"

namespace Hideo::Shell {

void MockLauncher::launch(State &s) {
    auto instance = makeRc<MockInstance>(
        icon,
        name,
        ramp
    );
    s.instances.emplaceFront(instance);
}

Ui::Child MockInstance::build() const {
    return Kr::scaffold({
               .icon = icon,
               .title = name,
               .body = [name = this->name] {
                   return Ui::labelMedium(name) | Ui::center();
               },
           }) |
           Ui::box({
               .backgroundFill = Ui::GRAY950,
           });
}

} // namespace Hideo::Shell
