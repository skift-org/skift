#pragma once

#include <libutils/Func.h>
#include <libutils/OwnPtr.h>
#include <libutils/Vector.h>

namespace Async
{

template <typename T>
struct Observable
{
public:
    struct _Observer
    {
        Observable<T> *_observable;
        Func<void(T &)> _callback;

        NONCOPYABLE(_Observer);
        NONMOVABLE(_Observer);

        _Observer(Observable<T> *observable, Func<void(T &)> callback)
            : _observable(observable),
              _callback(callback)
        {
            _observable->register_observer(this);
        }

        ~_Observer()
        {
            if (_observable)
            {
                _observable->unregister_observer(this);
            }
        }

        void detach()
        {
            _observable = nullptr;
            _callback = nullptr;
        }

        void notify(T &subject)
        {
            if (_callback)
            {
                _callback(subject);
            }
        }
    };

private:
    NONCOPYABLE(Observable);
    NONMOVABLE(Observable);

    Vector<_Observer *> _observers{};

    void register_observer(_Observer *ob)
    {
        if (!_observers.contains(ob))
        {
            _observers.push_back(ob);
        }
    }

    void unregister_observer(_Observer *ob)
    {
        _observers.remove_value(ob);
    }

public:
    Observable()
    {
    }

    virtual ~Observable()
    {
        for (size_t i = 0; i < _observers.count(); i++)
        {
            _observers[i]->detach();
        }
    }

    void did_update()
    {
        Vector<_Observer *> observers_copy{_observers};

        for (size_t i = 0; i < observers_copy.count(); i++)
        {
            observers_copy[i]->notify(*static_cast<T *>(this));
        }
    }

    OwnPtr<_Observer> observe(Func<void(T &)> callback)
    {
        return own<_Observer>(this, callback);
    }
};

template <typename T>
using Observer = typename Observable<T>::_Observer;

} // namespace Async
