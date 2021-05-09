#pragma once

#include <libutils/OwnPtr.h>
#include <libutils/RefPtr.h>
#include <libutils/Vector.h>
#include <libwidget2/Element.h>

namespace Widget2
{

struct BaseTransient
{
    virtual ~BaseTransient() {}

    virtual RefPtr<BaseElement> create() = 0;
    virtual bool match(BaseElement &element) = 0;
    virtual void inject(BaseElement &element) = 0;
};

template <typename TElement>
struct Transient : public BaseTransient
{
    using ElementType = TElement;
    using PropsType = typename TElement::PropsType;

private:
    PropsType _props;
    Vector<OwnPtr<BaseTransient>> _childs;

public:
    const PropsType &props() const { return _props; }

    const Vector<OwnPtr<BaseTransient>> &childs() const { return _childs; }

    Transient(PropsType props, Vector<OwnPtr<BaseTransient>> childs)
        : _props{props},
          _childs{childs}
    {
    }

    RefPtr<BaseElement> create() override
    {
        return make<TElement>(_props);
    }

    bool match(BaseElement &element) override
    {
        return dynamic_cast<ElementType *>(&element) != nullptr;
    }

    void inject(BaseElement &base_element) override
    {
        auto &element = static_cast<ElementType &>(base_element);

        if (_props != element.props())
        {
            element.inject(_props);
        }
    }
};

template <typename TElement>
OwnPtr<BaseTransient> make_transient(
    typename TElement::PropsType props,
    Vector<OwnPtr<BaseTransient>> childs)
{
    return own<Transient<TElement>>(props, childs);
}

} // namespace Widget2
