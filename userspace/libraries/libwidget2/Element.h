#pragma once

#include <libutils/RefCounted.h>

#include <libwidget2/Events.h>

namespace Widget2
{

struct BaseElement : public RefCounted<BaseElement>
{
private:
    Vector<RefPtr<BaseElement>> _childs;

public:
    virtual ~BaseElement() {}

    Vector<RefPtr<BaseElement>> &child() { return _childs; }

    virtual void dispatch(Event &) {}
};

template <typename TProps>
struct Element : public BaseElement
{
    using PropsType = TProps;

private:
    TProps _props{};

public:
    const TProps &props() const
    {
        return _props;
    }

    virtual void props_changed(const TProps &, const TProps &) {}

    Element(TProps props) : _props{props}
    {
    }

    void inject(TProps props)
    {
        if (_props != props)
        {
            props_changed(_props, props);
            _props = props;
        }
    }
};

template <typename TElement>
RefPtr<BaseElement> make_element(typename TElement::PropsType props)
{
    return make<TElement>(props);
}

} // namespace Widget2
