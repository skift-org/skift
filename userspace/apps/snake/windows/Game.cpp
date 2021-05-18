#include <libwidget/Components.h>

#include "snake/widgets/Board.h"
#include "snake/windows/Game.h"

using namespace Widget;
using namespace Graphic;

namespace Snake
{

RefPtr<Element> Game::build()
{
    return vflow({
        titlebar(Icon::get("snake"), "Snake"),
        board(),
    });
}

} // namespace Snake
