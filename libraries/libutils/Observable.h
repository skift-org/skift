#pragma once

#include <libutils/Callback.h>
#include <libutils/OwnPtr.h>
#include <libutils/Vector.h>

template <typename T>
class Observable
{
public:
    struct _Observer
    {
        Observable<T> *_observable;
        Callback<void(T &)> _callback;

        __noncopyable(_Observer);
        __nonmovable(_Observer);

        _Observer(Observable<T> *observable, Callback<void(T &)> callback)
            : _observable(observable),
              _callback(callback)
        {
            _observable->register_observer(this);
        }

        ~_Observer()
        {
            if (_observable)
                _observable->unregister_observer(this);
        }

        void detach()
        {
            _observable = nullptr;
        }

        void notify(T &subject)
        {
            _callback(subject);
        }
    };

private:
    __noncopyable(Observable);
    __nonmovable(Observable);

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

    OwnPtr<_Observer> observe(Callback<void(T &)> callback)
    {
        return own<_Observer>(this, callback);
    }
};

template <typename T>
using Observer = typename Observable<T>::_Observer;
