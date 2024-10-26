#pragma once

#include <karm-ui/node.h>
#include <vaev-markup/dom.h>

namespace Vaev::Tools {

Ui::Child inspector(Mime::Url url, Res<Strong<Vaev::Markup::Document>> dom);

} // namespace Vaev::Tools
