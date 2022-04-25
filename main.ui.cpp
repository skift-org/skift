#include <karm-base/box.h>
#include <karm-base/cons.h>
#include <karm-base/func.h>
#include <karm-base/rc.h>
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

struct _Props
{
};

template <Meta::Derive<_Prop>... Ts>
struct Props : _Props
{
};

struct _Node : public Base::Cons<Base::OptStrong<_Node>, Base::OptStrong<_Node>>
{
    Base::OptWeak<_Node> parent;
};

struct Node : public _Node
{
};

struct _Trans
{
    virtual ~_Trans() = default;

    virtual Base::Vec<Base::Box<struct _Trans>> build() = 0;

    virtual Base::Strong<struct _Node> realize() = 0;
};

template <typename N, typename... Ps>
struct Trans : _Trans
{
    Base::Strong<struct _Node> realize() override
    {
        return Base::make_strong<N>();
    }
};

template <typename N, typename... Ps>
struct BaseTrans : public Trans<N, Ps...>
{
};

template <typename N, typename... Ps>
struct FuncTrans : public Trans<N, Ps...>
{
};

template <typename N, typename... Props>
Base::Box<struct _Trans> make_trans(Base::Vec<Base::Box<_Trans>> &&children, Props... props);

template <typename N, typename... Props>
Base::Box<struct _Trans> make_trans(Base::Func<Base::Vec<Base::Box<_Trans>>()> build, Props... pros);

Base::Strong<_Node> make_root()
{
    return Base::make_strong<_Node>();
}

struct Engine
{
    void render(Base::Strong<struct _Node> &container, Base::Box<struct _Trans> &element)
    {
        Base::Strong<struct _Node> node = element->realize();
        Base::Vec<Base::Box<struct _Trans>> children = element->build();

        for (auto &child : children)
        {
            render(node, child);
        }

        container->children.push(std::move(node));
    }
};

}; // namespace Karm::Ui

template<typename T>
void $();

int main()
{
    auto app = $<Div>({
        
    });

    Karm::Ui::Engine engine;
    auto root = Karm::Ui::make_root();
    auto app = Karm::Ui::make_trans<Karm::Ui::Node>();

    engine.render(root, app);
}
