#include <karm-base/box.h>
#include <karm-base/vec.h>

namespace Karm::Ui
{

struct _Prop
{
};

template <typename T>
struct Prop
{
    T _value = {};
};

using BoxNode = Base::Box<struct _Node>;
struct _Node
{
    Base::Vec<BoxNode> children;
};

struct Node
{
};

using BoxTrans = Base::Box<struct _Trans>;
struct _Trans
{
    Base::Vec<BoxTrans> children;
};

template <typename... Props>
struct Trans
{
};

BoxTrans make(Base::Vec<BoxTrans> &&children)
{
    _Trans trans{.children = std::forward<Base::Vec<BoxTrans>>(children)};

    return BoxTrans::make(std::move(trans));
}

BoxNode make_root()
{
    _Node node{};
    return BoxNode::make(std::move(node));
}

struct Engine
{
    void render(BoxNode &container, BoxTrans &element)
    {
        BoxNode node = BoxNode::make();

        for (auto &child : element->children)
        {
            render(node, child);
        }

        container->children.push(std::move(node));
    }
};

}; // namespace Karm::Ui

int main()
{
    Karm::Ui::Engine engine;
    Karm::Ui::BoxNode root = Karm::Ui::make_root();
    Karm::Ui::BoxTrans app = Karm::Ui::make({});

    engine.render(root, app);
}
