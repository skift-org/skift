#include <karm-base/box.h>
#include <karm-base/cons.h>
#include <karm-base/func.h>
#include <karm-base/rc.h>
#include <karm-base/tuple.h>
#include <karm-base/vec.h>
#include <karm-meta/id.h>

namespace Karm::Ui
{

struct _Prop
{
};

// Represents a collection of properties.

struct _Props
{
    virtual ~_Props() = default;

    virtual _Prop *_get(Meta::TypeId id) = 0;

    virtual void _set(Meta::TypeId id, _Prop *prop) = 0;

    template <typename T>
    Base::Opt<Base::Ref<T>> get()
    {
        auto prop = _get(Meta::type_id<T>());

        if (!prop)
        {
            return Base::NONE;
        }

        return prop;
    }

    template <typename T>
    void set(T &prop)
    {
        _set(Meta::type_id<T>(), &prop);
    }
};

template <typename... Ts>
struct Props : public _Props
{
    Base::Tuple<Ts...> _buf;

    _Prop *_get(Meta::TypeId id) override
    {
        _Prop *result = nullptr;

        _buf.visit(
            [&]<typename T>(auto &i)
            {
                if (Meta::type_id<T>() == id)
                {
                    result = &i;
                }
            });

        return result;
    }

    void _set(Meta::TypeId id, _Prop *prop) override
    {
        _buf.visit(
            [&]<typename T>(auto &i)
            {
                if (Meta::type_id<T>() == id)
                {
                    i = *reinterpret_cast<T *>(prop);
                }
            });
    }
};

struct _Node
{
    virtual ~_Node() = default;

    virtual _Props *props() = 0;
};

template <typename... Ts>
struct Node : public _Node
{
    Props<Ts...> _props;

    _Props *props() override { return &_props; }
};

using Frag = Base::Vec<Base::Box<struct _Trans>>;

struct _Trans
{
    virtual ~_Trans() = default;

    virtual _Props *props() = 0;

    virtual Frag children() = 0;
};

template <typename... Ts>
struct Trans : public _Trans
{
    Props<Ts...> _props;

    Trans(Props<Ts...> &&props) : _props(std::forward<Props<Ts...>>(props)){};

    _Props *props() override
    {
        return &_props;
    }
};

template <typename N, typename... Ts>
struct StaticTrans : public Trans<N, Ts...>
{
    Frag _children;

    StaticTrans(Props<Ts...> &&props, Frag &&children)
        : Trans<N, Ts...>(std::forward<Props<Ts...>>(props)),
          _children(std::forward<Frag>(children))
    {
    }

    Frag children() override
    {
        return std::move(_children);
    }
};

template <typename... Ts>
struct FuncTrans : public Trans<Ts...>
{
    using F = Base::Func<Frag(Props<Ts...>)>;

    F _build;

    FuncTrans(Props<Ts...> &&props, F &&build)
        : Trans<_Node, Ts...>(std::forward<Props<Ts...>>(props)),
          _build(std::forward<Base::Func<Frag(_Props &)>>(build))
    {
    }

    Frag children() override
    {
        return _build(this->_props);
    }
};

template <typename N, typename... Ts>
Base::Box<_Trans> $<N>(Props<Ts...> props = {}, Frag children = {})
{
    return Base::make_box<StaticTrans<N, Ts...>>(std::forward<Props<Ts...>>(props), std::forward<Frag>(children));
}

template <typename... Ts>
Base::Box<_Trans> $(Props<Ts...> props = {}, Base::Func<Frag(Props<Ts...>)> build = {})
{
    return Base::make_box<StaticTrans<Ts...>>(std::forward<Props<Ts...>>(props), std::forward<Frag>(build));
}

}; // namespace Karm::Ui

using namespace Karm::Ui;

int main()
{
    auto app = $<Node<>>({});
}
