module;

#include <karm-base/rc.h>
#include <karm-base/res.h>

export module Karm.Ui:_embed;

namespace Karm::Ui {

struct Node;

struct Host;

} // namespace Karm::Ui

namespace Karm::Ui::_Embed {

Res<Rc<Host>> makeHost(Rc<Node> root);

} // namespace Karm::Ui::_Embed
